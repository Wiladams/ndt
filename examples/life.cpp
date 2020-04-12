#include "p5.hpp"

/*
    Game of life as a single graphic
       https://p5js.org/examples/simulate-game-of-life.html
*/
int lwidth = 1280;
int lheight = 1024;




int ** Array2D(int rows, int columns)
{
    int **arr = new int *[rows];
    for (int i=0; i<rows; i++) {
        arr[i] = new int[columns];
    }

    return arr;
}

static const int w = 10;  // size of a cell

class Life {
    int fW;
    int fColumns;
    int fRows;

    int **board;
    int **next;
    int **neighborHood;

public:
    Life(int aWidth, int aHeight)
    {
        fW = w;

        // Calculate columns and rows
        fColumns = floor(aWidth/fW);
        fRows = floor(aHeight/fW);

//printf("rows: %d  cols: %d\n", fRows, fColumns);

        board = Array2D(fRows, fColumns);
        next = Array2D(fRows, fColumns);
        neighborHood = Array2D(fRows-2, fColumns-2);

        init();
    }

    // Fill board randomly
    void init() 
    {
        for (int row = 0; row<fRows; row++) {
            for (int column = 0; column < fColumns; column++) {
                // Lining the edges with 0s
                if ((column == 0) || (row == 0) || (column == fColumns-1) || (row == fRows-1)) {
                    board[row][column] = 0;
                } else { 
                    int val = (int)random(0,1);
                    //printf("%3d ", val);
                    board[row][column] = val;
                }
                next[row][column] = 0;
            }
            //printf("\n");
        }
    }

    // The process of creating the new generation
    void generate()
    {

        // Loop through every spot in our 2D array and check spots neighbors
        for (int y = 1; y < fRows - 1; y++) {
            for (int x = 1; x<fColumns - 1; x++) {

                // Add up all the states in a 3x3 surrounding grid
                int neighbors = 0;
                for (int i = -1; i<= 1; i++) {
                    for (int j = -1; j<= 1; j++) {
                        neighbors = neighbors + board[y+j][x+i];
                    }
                }

                // A little trick to subtract the current cell's state since
                // we added it in the above loop
                neighbors = neighbors - board[y][x];
                //neighborHood[x-1][y-1] = neighbors
                //printf("%3d ", neighbors);


                // Rules of Life
                if ((board[y][x] == 1) && (neighbors <  2)) {
                    next[y][x] = 0;           // Loneliness
                } else if ((board[y][x] == 1) && (neighbors >  3)) {
                    next[y][x] = 0;           // Overpopulation
                } else if ((board[y][x] == 0) && (neighbors == 3)) {
                    next[y][x] = 1;           // Reproduction
                } else {                                          
                    next[y][x] = board[y][x]; // Stasis
                }

            }
            //printf("\n");
        }

        // Swap!
        int **temp = board;
        board = next;
        next = temp;
    }


    void draw()
    {
        generate();
        noStroke();

        for (int i = 0; i <= fColumns-1; i++) {
            for (int j = 0; j<=fRows-1;j++) {
                if (board[j][i] == 1) {
                    fill(255);
                } else { 
                    if (!gIsLayered) {
                        fill(0); 
                    } else {
                        fill(0,0);
                    }
                }

                rect(i*fW, j*fW, fW, fW);
            }
        }
    }
};

Life l1(lwidth,lheight);

int T_SP = ' ';

void keyTyped(const KeyEvent &event)
{
    // If the user types a '<sp>' reset
    // the chain to 1 node
    if (keyCode == T_SP) {
        if (gIsLayered)
            noLayered();
        else
            layered();
    } else {
        l1.init();
    }
}

void draw()
{
    if (!gIsLayered) {
        background(255);
    } else {
        clear();
    }

    l1.draw();
}

void setup()
{
    createCanvas(lwidth, lheight);
    frameRate(60);
}