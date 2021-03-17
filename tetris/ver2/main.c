//compile: 
//gcc -w -o main main.c

#include <stdio.h>
#include <stdbool.h>
#include <windows.h>
#include <conio.h>
#include "tetris.h"

//Game constants
#define FPS 60

//Game phases
void Init();
void EarlyUpdate();
void Update();
void LateUpdate();

gameplay player;

int main()
{
    player.gravity.g = 23;
    vector2d_assign(&player.tetrominoes.pos[0], 5, 5);
    vector2d_assign(&player.tetrominoes.pos[1], 4, 5);
    vector2d_assign(&player.tetrominoes.pos[2], 3, 5);
    vector2d_assign(&player.tetrominoes.pos[3], 4, 4);

    Init();

    //Game loop
    while (!player.isFinish) {
        EarlyUpdate();
        Update();
        LateUpdate();
        
        Sleep(1000/FPS);
    }
}

void Init()
{
    gameplay_init(&player);
    gravity_init(&player.gravity);
    controls_init(&player.controls);
}

void EarlyUpdate() 
{
    board_refresh(player.main.board);
    tetrominoes_controls(&player);
}

void Update()
{
    tetrominoes_gravity(&player);
}

void LateUpdate()
{
    board_sync(player.main.board, player.main.fixed, player.tetrominoes.pos);
    scrn_prnt(player.main.board);
}