#include "p5.hpp"

#include "rsw_math.h"

#define MANGLE_TYPES
#include "gltools.h"

#define PORTABLEGL_IMPLEMENTATION
#include "GLObjects.h"

constexpr int WIDTH = 640;
constexpr int HEIGHT = 480;

using namespace std;
//using namespace p5;

using rsw::vec4;
using rsw::vec3;
using rsw::vec2;
using rsw::mat4;

typedef struct My_Uniforms
{
	mat4 mvp_mat{};
	GLuint tex{};
	vec4 v_color{};
	float time{};

} My_Uniforms;

bool handle_events();
void cleanup();
void setup_context();
void setup_gl_data();



void normal_vs(float* vs_output, void* vertex_attribs, Shader_Builtins* builtins, void* uniforms);
void normal_fs(float* fs_input, Shader_Builtins* builtins, void* uniforms);

void texture_replace_vs(float* vs_output, void* vertex_attribs, Shader_Builtins* builtins, void* uniforms);
void texture_replace_fs(float* fs_input, Shader_Builtins* builtins, void* uniforms);
void tex_rect_fs(float* fs_input, Shader_Builtins* builtins, void* uniforms);

void tex_array_vs(float* vs_output, void* vertex_attribs, Shader_Builtins* builtins, void* uniforms);
void tex_array_fs(float* fs_input, Shader_Builtins* builtins, void* uniforms);

vec4 Red{ 1.0f, 0.0f, 0.0f, 0.0f };
vec4 Green{ 0.0f, 1.0f, 0.0f, 0.0f };
vec4 Blue{ 0.0f, 0.0f, 1.0f, 0.0f };


glContext the_Context;

My_Uniforms the_uniforms;
int tex_index;
int tex_filter;

constexpr size_t NUM_TEXTURES = 5;
GLuint textures[NUM_TEXTURES];

GLuint tex_array_shader;
GLuint texture_replace;
GLuint tex_rect_shader;

mat4 scale_mat, rot_mat;
int frames;


void normal_vs(float* vs_output, void* vertex_attribs, Shader_Builtins* builtins, void* uniforms)
{
	*(vec4*)&builtins->gl_Position = *((mat4*)uniforms) * ((vec4*)vertex_attribs)[0];
}

void normal_fs(float* fs_input, Shader_Builtins* builtins, void* uniforms)
{
	*(vec4*)&builtins->gl_FragColor = ((My_Uniforms*)uniforms)->v_color;
}

void texture_replace_vs(float* vs_output, void* vertex_attribs, Shader_Builtins* builtins, void* uniforms)
{
	My_Uniforms* u = (My_Uniforms*)uniforms;
	((vec2*)vs_output)[0] = ((vec4*)vertex_attribs)[2].xy(); //tex_coords

	*(vec4*)&builtins->gl_Position = u->mvp_mat * ((vec4*)vertex_attribs)[0];

}

void texture_replace_fs(float* fs_input, Shader_Builtins* builtins, void* uniforms)
{
	vec2 tex_coords = ((vec2*)fs_input)[0];
	GLuint tex = ((My_Uniforms*)uniforms)->tex;


	builtins->gl_FragColor = texture2D(tex, tex_coords.x, tex_coords.y);
	//print_vec4(stdout, builtins->gl_FragColor, "\n");
}


void draw()
{
	static int counter = 0;

	double orig_time = p5::millis();
	double old_time = 0, new_time = 0;

	++counter;
	new_time = p5::millis();
	if (new_time - old_time >= 3000) {
		//printf("%f FPS\n", p5::getFrameCount()/p5::seconds());
		old_time = new_time;
		counter = 0;
	}

	// TODO time/depth 0-1 or 0-(frames-1)?
	// the former if I tried to use texture3D, the latter for texture2dArray
	the_uniforms.time = ((int((new_time - orig_time) / 50)) % frames);// /(float)frames;


	glClear(GL_COLOR_BUFFER_BIT);
	//glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	
	//Render the scene
	// done automatically after drawing
}


void update(double t)
{
	static unsigned int last_time = 0, cur_time;

	cur_time = p5::millis();
	float time = (cur_time - last_time) / 1000.0f;

#define MOVE_SPEED DEG_TO_RAD(30)

	mat4 tmp;

	if (p5::keyStates[VK_LEFT]) {
		rsw::load_rotation_mat4(tmp, vec3(0, 0, 1), time * MOVE_SPEED);
		the_uniforms.mvp_mat = the_uniforms.mvp_mat * tmp;
	}
	if (p5::keyStates[VK_RIGHT]) {
		rsw::load_rotation_mat4(tmp, vec3(0, 0, 1), -time * MOVE_SPEED);
		the_uniforms.mvp_mat = the_uniforms.mvp_mat * tmp;
	}
	if (p5::keyStates[VK_UP]) {
		the_uniforms.mvp_mat = the_uniforms.mvp_mat * rsw::scale_mat4(1.01, 1.01, 1);
	}
	if (p5::keyStates[VK_DOWN]) {
		the_uniforms.mvp_mat = the_uniforms.mvp_mat * rsw::scale_mat4(0.99, 0.99, 1);
	}

	last_time = cur_time;

}

GLenum smooth[2] = { SMOOTH, SMOOTH };

float points[] =
{
	-0.5,  0.5, -0.1,
	-0.5, -0.5, -0.1,
	 0.5,  0.5, -0.1,
	 0.5, -0.5, -0.1
};

float tex_coords[] =
{
	0.0, 0.0,
	0.0, 1.0,
	1.0, 0.0,
	1.0, 1.0,
	0.0, 0.0,
	0.0, 511.0,
	511.0, 0.0,
	511.0, 511.0
};

void setup()
{
	p5::createCanvas(WIDTH, HEIGHT, "Texturing");

	// because canvasPixelData itself is not an lvalue
	auto p = (uint32_t*)canvasPixelData;
	if (!init_glContext(&the_Context, &p, WIDTH, HEIGHT, 32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000)) {
		puts("Failed to initialize glContext");
		exit(0);
	}
	set_glContext(&the_Context);

	
	//can't turn off C++ destructors
	{
		Pixel test_texture[9];
		for (int i = 0; i < 9; ++i) {
			if (i % 2)
				test_texture[i] = Pixel(0, 0, 0, 255);
			else
				test_texture[i] = Pixel(255, 255, 255, 255);
		}
		/*
		glGenTextures(NUM_TEXTURES, textures);
		glBindTexture(GL_TEXTURE_2D, textures[0]);
		if (!load_texture2D("./media/textures/test1.jpg", GL_NEAREST, GL_NEAREST, GL_MIRRORED_REPEAT, false, false)) {
			puts("failed to load texture");
			halt();
		}


		glBindTexture(GL_TEXTURE_2D, textures[1]);

		if (!load_texture2D("./media/textures/test2.jpg", GL_NEAREST, GL_NEAREST, GL_REPEAT, false, false)) {
			puts("failed to load texture");
			halt();
		}

		glBindTexture(GL_TEXTURE_2D, textures[2]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_COMPRESSED_RGBA, 3, 3, 0, GL_RGBA, GL_UNSIGNED_BYTE, test_texture);

		glBindTexture(GL_TEXTURE_2D_ARRAY, textures[3]);


		if (!(frames = load_texture2D_array_gif("./media/textures/Nathan_Fillion_awesome_smaller2.gif", GL_NEAREST, GL_NEAREST, GL_REPEAT))) {
			puts("failed to load texture");
			halt();
		}

		glBindTexture(GL_TEXTURE_RECTANGLE, textures[4]);
		if (!load_texture_rect("./media/textures/tex04.jpg", GL_NEAREST, GL_NEAREST, GL_REPEAT, false)) {
			puts("failed to load texture");
			halt();
		}

		mat4 identity;

		Buffer square(1);
		square.bind(GL_ARRAY_BUFFER);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 12, points, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

		Buffer tex_buf(1);
		tex_buf.bind(GL_ARRAY_BUFFER);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 16, tex_coords, GL_STATIC_DRAW);
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);


		GLuint normal_shader = pglCreateProgram(normal_vs, normal_fs, 0, NULL, GL_FALSE);
		glUseProgram(normal_shader);
		pglSetUniform(&the_uniforms);

		tex_array_shader = pglCreateProgram(tex_array_vs, tex_array_fs, 2, smooth, GL_FALSE);
		glUseProgram(tex_array_shader);
		pglSetUniform(&the_uniforms);

		tex_rect_shader = pglCreateProgram(texture_replace_vs, tex_rect_fs, 2, smooth, GL_FALSE);
		glUseProgram(tex_rect_shader);
		pglSetUniform(&the_uniforms);

		texture_replace = pglCreateProgram(texture_replace_vs, texture_replace_fs, 2, smooth, GL_FALSE);
		glUseProgram(texture_replace);
		pglSetUniform(&the_uniforms);


		the_uniforms.v_color = Red;
		the_uniforms.mvp_mat = identity;

		tex_index = 0;
		tex_filter = 0;
		the_uniforms.tex = textures[tex_index];
		*/

		glClearColor(1, 0, 0, 1);

	}

	update(0);
}

void keyDown(const KeyboardEvent& e)
{
	switch (e.keyCode)
	{
	case VK_ESCAPE:
		halt();
		break;
	case '1':
		tex_index = ((tex_index + 1) % NUM_TEXTURES);
		the_uniforms.tex = textures[tex_index];
		if (tex_index == NUM_TEXTURES - 2) {
			glUseProgram(tex_array_shader);
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);
		}
		else if (tex_index == NUM_TEXTURES - 1) {
			glUseProgram(tex_rect_shader);
			pglVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, sizeof(GLfloat) * 8);
		}
		else {
			glUseProgram(texture_replace);
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);
		}
		break;

	case 'F':
	{
		int filter;
		if (tex_filter == 0) {
			puts("Switching to GL_LINEAR");
			filter = GL_LINEAR;
		}
		else {
			puts("Switching to GL_NEAREST");
			filter = GL_NEAREST;
		}
		for (int i = 0; i < NUM_TEXTURES - 2; ++i) {
			glBindTexture(GL_TEXTURE_2D, textures[i]);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
		}
		glBindTexture(GL_TEXTURE_2D_ARRAY, textures[NUM_TEXTURES - 2]);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, filter);

		glBindTexture(GL_TEXTURE_RECTANGLE, textures[NUM_TEXTURES - 1]);
		glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, filter);

		tex_filter = !tex_filter;
	}
	break;
	default:
		;
	}
}

// Various work routines

void tex_rect_fs(float* fs_input, Shader_Builtins* builtins, void* uniforms)
{
	vec2 tex_coords = ((vec2*)fs_input)[0];
	GLuint tex = ((My_Uniforms*)uniforms)->tex;


	builtins->gl_FragColor = texture_rect(tex, tex_coords.x, tex_coords.y);
	//print_vec4(stdout, builtins->gl_FragColor, "\n");
}

void tex_array_vs(float* vs_output, void* vertex_attribs, Shader_Builtins* builtins, void* uniforms)
{
	My_Uniforms* u = (My_Uniforms*)uniforms;
	((vec2*)vs_output)[0] = ((vec4*)vertex_attribs)[2].xy(); //uv tex_coords (layer is uniform in fs)

	*(vec4*)&builtins->gl_Position = u->mvp_mat * ((vec4*)vertex_attribs)[0];

}

void tex_array_fs(float* fs_input, Shader_Builtins* builtins, void* uniforms)
{
	My_Uniforms* u = (My_Uniforms*)uniforms;
	vec3 tex_coords = { fs_input[0], fs_input[1], u->time };
	GLuint tex = u->tex;

	builtins->gl_FragColor = texture2DArray(tex, tex_coords.x, tex_coords.y, tex_coords.z);
	//print_vec4(builtins->gl_FragColor, "\n");
}
