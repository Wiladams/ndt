/*
	Demonstrate screen capture and recording.
*/
#include "p5.hpp"
#include "elements/screensnapshot.hpp"

#include <memory>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>


using namespace p5;

ScreenSnapper snapper;


#include <stdio.h>
#include <stdlib.h>

static void solve_least_squares(const float A[][9], const int rows, const int cols, const float quad2[][2], float M[]) {
    // Create augmented matrix
    float augmented[9][10] = { {0} };
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            augmented[i][j] = A[i][j];
        }
        augmented[i][cols] = quad2[i / 2][i % 2];
    }

    // Perform Gaussian elimination to solve for M
    for (int i = 0; i < rows; i++) {
        // Find pivot row and swap
        int max_row = i;
        for (int k = i + 1; k < rows; k++) {
            if (fabs(augmented[k][i]) > fabs(augmented[max_row][i])) {
                max_row = k;
            }
        }
        for (int k = i; k <= cols; k++) {
            float temp = augmented[max_row][k];
            augmented[max_row][k] = augmented[i][k];
            augmented[i][k] = temp;
        }

        // Make all rows below this one 0 in current column
        for (int k = i + 1; k < rows; k++) {
            float factor = -augmented[k][i] / augmented[i][i];
            for (int j = i; j <= cols; j++) {
                if (i == j) {
                    augmented[k][j] = 0;
                }
                else {
                    augmented[k][j] += factor * augmented[i][j];
                }
            }
        }
    }

    // Back substitution
    for (int i = rows - 1; i >= 0; i--) {
        M[i] = augmented[i][cols];
        for (int j = i + 1; j < cols; j++) {
            M[i] -= augmented[i][j] * M[j];
        }
        M[i] /= augmented[i][i];
    }
}

// Rest of the code remains the same as before...
static const int N_POINTS = 4;
static const int N_ROWS = 8;
static const int N_COLS = 9;


static void construct_matrix(const float quad1[N_POINTS][2], const float quad2[N_POINTS][2], float matrix[3][3]) {
    float A[N_ROWS][N_COLS] = { {0} };
    float M[N_COLS] = { 0 };

    // Construct matrix A
    for (int i = 0; i < N_POINTS; i++) {
        float x = quad1[i][0];
        float y = quad1[i][1];
        float u = quad2[i][0];
        float v = quad2[i][1];

        A[i * 2][0] = x;
        A[i * 2][1] = y;
        A[i * 2][2] = 1;
        A[i * 2 + 1][3] = x;
        A[i * 2 + 1][4] = y;
        A[i * 2 + 1][5] = 1;
        A[i * 2][6] = -u * x;
        A[i * 2][7] = -u * y;
        A[i * 2][8] = -u;
        A[i * 2 + 1][6] = -v * x;
        A[i * 2 + 1][7] = -v * y;
        A[i * 2 + 1][8] = -v;
    }

    // Solve for the transformation matrix M
    // Here you should use a linear algebra library or implement your own least squares solver
    // For simplicity, I'll just use a placeholder function here
    solve_least_squares(A, N_ROWS, N_COLS, quad2, M);

    // Append 1 to get a 3x3 matrix
    M[8] = 1.0;

    // Reshape M to 3x3
    int k = 0;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            matrix[i][j] = M[k++];
        }
    }
}



int quads_to_matrix(const float quad1[N_POINTS][2], const float quad2[N_POINTS][2])
{
    //float quad1[N_POINTS][2] = { {0, 0}, {1, 0}, {1, 1}, {0, 1} }; // Define the first quadrilateral
    //float quad2[N_POINTS][2] = { {2, 2}, {4, 2}, {4, 4}, {2, 4} }; // Define the second quadrilateral

    float matrix[3][3] = { {0} };

    construct_matrix(quad1, quad2, matrix);

    // Print the resulting matrix
    printf("Resulting matrix:\n");
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            printf("%.2f ", matrix[i][j]);
        }
        printf("\n");
    }

    return 0;
}




void draw()
{
	snapper.next();

	// display full size
	//image(snapper.getImage(), 0, 0);
	
	// Scale the full image to fit within our quarter
	// sized canvas
	scaleImage(snapper.getImage(), 0, 0, snapper.getImage().width(), snapper.getImage().height(), 0, 0, canvasWidth, canvasHeight);
}

void keyReleased(const KeyboardEvent& e)
{
	switch (e.keyCode)
	{
	case VK_ESCAPE:
		halt();
		break;
	}
}

void setup()
{
	frameRate(30);
	
	// Create a canvas that is quarter size
	createCanvas(displayWidth/2, displayHeight/2, "screenview", 4);

	snapper.reset(0,0,displayWidth, displayHeight);
}

