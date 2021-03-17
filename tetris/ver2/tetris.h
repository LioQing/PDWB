#ifndef TETRIS_UTIL
#define TETRIS_UTIL

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <windows.h>

//Game constants
#define BOARD_HEIGHT 20
#define BOARD_WIDTH 10

//dir dataype
const enum dirCode{UP, DOWN, LEFT, RIGHT} dirCode;
typedef struct dir {
    bool dir[4];
} dir;

//vector2d datatype
typedef struct vector2d {
    int x;
    int y;
} vector2d;

const vector2d vector2d_up = {0, -1};
const vector2d vector2d_down = {0, 1};
const vector2d vector2d_left = {-1, 0};
const vector2d vector2d_right = {1, 0};

//Basic vector2d func
void vector2d_to_board(int board[BOARD_WIDTH][BOARD_HEIGHT], vector2d pos, int value) {
    board[pos.x][pos.y] = value;
}

void vector2d_assign(vector2d *dest, int x, int y) {
    dest->x = x;
    dest->y = y;
}

void vector2d_move(vector2d *dest, vector2d vel) {
    vector2d_assign(dest, dest->x + vel.x, dest->y + vel.y);
}

int vector2d_to_dir(vector2d src) {
    if (src.x > 0 && src.y == 0) {
        return RIGHT;
    } else if (src.x < 0 && src.y == 0) {
        return LEFT;
    } else if (src.x == 0 && src.y > 0) {
        return DOWN;
    } else if (src.x == 0 && src.y < 0) {
        return UP;
    } else {
        return -1;
    }
}

//Gameplay datatype
typedef struct gameplay {
    struct main {
        int board[BOARD_WIDTH][BOARD_HEIGHT];
        int fixed[BOARD_WIDTH][BOARD_HEIGHT];
    } main;

    struct tetrominoes {
        vector2d pos[4];
    } tetrominoes;

    struct gravity {
        int g; //frame per block
        int timer;
    } gravity;

    struct controls {
        struct keys {
            int coolDown;
            int timer;
        } move;
        bool sideMove[2];
    } controls;

    bool isFinish;
} gameplay;

//Gameplay Init
void gameplay_init(gameplay *gameplay)
{
    gameplay->isFinish = false;
}

//Refresh board
void board_refresh(int board[BOARD_WIDTH][BOARD_HEIGHT])
{
    for (int h = 0; h < BOARD_HEIGHT; ++h) {
        for (int w = 0; w < BOARD_WIDTH; ++w) {
            board[w][h] = 0;
        }
    }
}

//Sync things onto the board
void board_sync(int board[BOARD_WIDTH][BOARD_HEIGHT], int fixed[BOARD_WIDTH][BOARD_HEIGHT], vector2d pos[4])
{
    for (int i = 0; i < 4; ++i) {
        vector2d_to_board(board, pos[i], 1);
    }

    for (int h = 0; h < BOARD_HEIGHT; ++h) {
        for (int w = 0; w < BOARD_WIDTH; ++w) {
            if (fixed[w][h] == 1) {
                board[w][h] = fixed[w][h];
            }
        }
    }
}

//Tetrominoes - move
void tetrominoes_move(vector2d pos[], vector2d vel)
{
    dir touch;
    tetrominoes_touch_border(pos, &touch);

    if (!touch.dir[vector2d_to_dir(vel)]) {
        for (int i = 0; i < sizeof(pos); ++i) {
            vector2d_move(&pos[i], vel);
        }
    }
}

//Tetrominoes - touching border
void tetrominoes_touch_border(vector2d pos[], dir *output)
{
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < sizeof(pos); ++j) {
            if (pos[i].y == 0  || !output->dir[UP])    output->dir[UP]    = true; else output->dir[UP]    = false;
            if (pos[i].y == 19 || !output->dir[DOWN])  output->dir[DOWN]  = true; else output->dir[DOWN]  = false;
            if (pos[i].x == 0  || !output->dir[LEFT])  output->dir[LEFT]  = true; else output->dir[LEFT]  = false;
            if (pos[i].x == 9  || !output->dir[RIGHT]) output->dir[RIGHT] = true; else output->dir[RIGHT] = false;
        }
    }
}

//Tetrominoes - Gravity
void tetrominoes_gravity(gameplay *gameplay)
{
    if (gameplay->gravity.timer >= gameplay->gravity.g) {
        tetrominoes_move(gameplay->tetrominoes.pos, vector2d_down);
        gameplay->gravity.timer = 0;
    } else {
        gameplay->gravity.timer++;
    }
}

//Gravity init
void gravity_init(struct gravity *gravity)
{
    gravity->timer = 0;
}

//Controls Init
void controls_init(struct controls *controls)
{
    controls->move.coolDown = 7;
    controls->move.timer = 0;
}

//Controls
void tetrominoes_controls(gameplay *gameplay)
{
    //Left and right
    if (GetAsyncKeyState(0x41) || GetAsyncKeyState(VK_LEFT)) {
        if (!gameplay->controls.sideMove[0]) {
            gameplay->controls.move.timer = 0;
            gameplay->controls.sideMove[0] = true;
        }

        if (gameplay->controls.move.timer++ <= 0 || gameplay->controls.move.timer > gameplay->controls.move.coolDown) {
            tetrominoes_move(gameplay->tetrominoes.pos, vector2d_left);
        }
    }
    else if (GetAsyncKeyState(0x44) || GetAsyncKeyState(VK_RIGHT)) {
        if (!gameplay->controls.sideMove[1]) {
            gameplay->controls.move.timer = 0;
            gameplay->controls.sideMove[1] = true;
        }

        if (gameplay->controls.move.timer++ <= 0 || gameplay->controls.move.timer > gameplay->controls.move.coolDown) {
            tetrominoes_move(gameplay->tetrominoes.pos, vector2d_right);
        }
    }

    if (!(GetAsyncKeyState(0x41) || GetAsyncKeyState(VK_LEFT))) {
        gameplay->controls.sideMove[0] = false;
    }
    if (!(GetAsyncKeyState(0x44) || GetAsyncKeyState(VK_RIGHT))) {
        gameplay->controls.sideMove[1] = false;
    }

    //Escape
    if (GetAsyncKeyState(VK_ESCAPE)) {
        gameplay->isFinish = true;
    }
}

//Print out the scrn
void scrn_prnt(int board[BOARD_WIDTH][BOARD_HEIGHT])
{
    unsigned char scrn_c[(BOARD_WIDTH * 2 + 1) * BOARD_HEIGHT + 1];
    int board_ptr = 0;

    for (int h = 0; h < BOARD_HEIGHT; ++h) {
        for (int w = 0; w < BOARD_WIDTH; ++w) {
            if (board[w][h] == 1) {
                scrn_c[board_ptr++] = 219;
                scrn_c[board_ptr++] = 219;
            } else {
                scrn_c[board_ptr++] = '.';
                scrn_c[board_ptr++] = ' ';
            }
        }
        scrn_c[board_ptr++] = '\n';
    }
    scrn_c[board_ptr++] = '\0';

    puts(scrn_c);
}

#endif