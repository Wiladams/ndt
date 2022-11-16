// More resources
//https://learnopengl.com/Getting-started/Hello-Triangle
//

#include "p5.hpp"

#include "rsw_math.h"

//#define MANGLE_TYPES
#include "gltools.h"

#define PORTABLEGL_IMPLEMENTATION
#include "GLObjects.h"

constexpr int WIDTH = 640;
constexpr int HEIGHT = 480;

using namespace std;


//using rsw::vec4;
//using rsw::vec3;
//using rsw::vec2;
//using rsw::mat4;


vec4 Red{ 1.0f, 0.0f, 0.0f, 0.0f };
vec4 Green{ 0.0f, 1.0f, 0.0f, 0.0f };
vec4 Blue{ 0.0f, 0.0f, 1.0f, 0.0f };

typedef struct My_Uniforms
{
	mat4 mvp_mat;
	vec4 v_color;
} My_Uniforms;


glContext the_Context{};
u32* gPixelData = nullptr;
My_Uniforms the_uniforms;


GLfloat vertices[] = { -1.0f,0.0f,0.0f,
						0.0f,1.0f,0.0f,
						0.0f,0.0f,0.0f };
GLfloat colors[] = { 1.0f, 0.0f, 0.0f,
						0.0f, 1.0f, 0.0f,
						0.0f, 0.0f, 1.0f };
GLfloat vertices2[] = { 0.0f,0.0f,0.0f,
						0.0f,-1.0f,0.0f,
						1.0f,0.0f,0.0f };


unsigned int vertexArrayObjID[2];
unsigned int vertexBufferObjID[3];


void draw()
{
	glClear(GL_COLOR_BUFFER_BIT);

	glBindVertexArray(vertexArrayObjID[0]);	// First VAO
	glDrawArrays(GL_TRIANGLES, 0, 3);		// draw first object

	// select second VAO
	glBindVertexArray(vertexArrayObjID[1]);		
	//glVertexAttrib3f((GLuint)1, 1.0, 0.0, 0.0); // set constant color attribute
	glDrawArrays(GL_TRIANGLES, 0, 3);	// draw second object

	glBindVertexArray(0);

}

void smooth_vs(float* vs_output, void* vertex_attribs, Shader_Builtins* builtins, void* uniforms)
{
	My_Uniforms* u = (My_Uniforms*)uniforms;

	vec4* v_attribs = (vec4 *)vertex_attribs;
	((vec4*)vs_output)[0] = v_attribs[1];	// color

	*(vec4*)&builtins->gl_Position = mult_mat4_vec4(u->mvp_mat, ((vec4*)vertex_attribs)[0]);
}

//
// Implementation of a simple fragment shader
//
// Tthe essential function of the fragment shader
// is to take an input color, and turn it to the output color
//
// The output color is stuffed into the gl_FragColor field of the 
// Shader_Builtins structure.
//
void smooth_fs(float* fs_input, Shader_Builtins* builtins, void* uniforms)
{
	// we have to do the funky type cast *(vec4*)&  because we don't
	// know the internal representation of the fragColor, it might
	// be some 'internal only' type
	*(vec4*)&builtins->gl_FragColor = ((vec4*)fs_input)[0];
	
	// If we want to use the color in the uniforms
	//*(vec4*)&builtins->gl_FragColor = ((My_Uniforms*)uniforms)->v_color;
}

GLenum interpolation[4] = { SMOOTH, SMOOTH, SMOOTH, SMOOTH };

void initShaders()
{
	GLuint myshader = pglCreateProgram(smooth_vs, smooth_fs, 4, interpolation, GL_FALSE);
	glUseProgram(myshader);

	mat4 identity{};

	the_uniforms.v_color = Red;
	SET_IDENTITY_MAT4(the_uniforms.mvp_mat);

	pglSetUniform(&the_uniforms);
}




void setupGL()
{
	// because canvasPixelData itself is not an lvalue
	gPixelData = (u32*)canvasPixelData;
	if (!init_glContext(&the_Context, &gPixelData, canvasWidth, canvasHeight, 32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000)) {
		puts("Failed to initialize glContext");
		exit(0);
	}
	set_glContext(&the_Context);

	initShaders();

	//glClearColor(0.75, 0.75, 0.75, 1);
	glClearColor(0.75, 0.75, 0.75,0.25);

	// Allocate vertex array objects
	glGenVertexArrays(2, &vertexArrayObjID[0]);

	// Setup first vertex array object
	glBindVertexArray(vertexArrayObjID[0]);
	glGenBuffers(2, vertexBufferObjID);

	// VBO for vertex data
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObjID[0]);
	glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(GLfloat), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	// VB for color data
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObjID[1]);
	glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(GLfloat), colors, GL_STATIC_DRAW);
	glVertexAttribPointer((GLuint)1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	// Setup second Vertex Array Object
	glBindVertexArray(vertexArrayObjID[1]);
	glGenBuffers(1, &vertexBufferObjID[2]);

	// VBO for vertex data
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObjID[2]);
	glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(GLfloat), vertices2, GL_STATIC_DRAW);
	glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);


	glViewport(0, 0, canvasWidth, canvasHeight);
	//glViewport(0, 0, canvasWidth/2, canvasHeight/2);
}

void setup()
{
	p5::createCanvas(WIDTH, HEIGHT, "portablegl");
	//layered();

	setupGL();

}

void keyDown(const KeyboardEvent& e)
{
	switch (e.keyCode)
	{
	case VK_ESCAPE:
		halt();
		break;

	default:
		;
	}
}


