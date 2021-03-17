#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <windows.h>
#include <winuser.h>
#include <math.h>
#include <conio.h>
#include <ctype.h>
#include <time.h>

#define fps 60
#define board_width 10
#define board_height 20
#define side_width 8

void win_size();

unsigned char margin[6][1] = {{201}, {205}, {187}, {186}, {200}, {188}};
bool g_fix = false;
bool over = false;

void CDReset();
void UIInitial();
void Menus();
void Render();
void Pause();
void PauseIni();
void End();

void GameInitial();
void EarlyUpdate();
void Update();
void LateUpdate();
void Finish();

int board[board_width][board_height] = {0};
void board_sync();

void scrn_prnt();

struct ui
{
    int menus; //0 - main, 1 - start, 2 - how to play, 3 - pause, 4 - end screen
    int main_selection;
    int pause_selection;
    int end_selection;
    int lastcd[6];
    int scrn[board_width + 4 + side_width * 2][board_height + 2];
    int startlevels[5];
    int lvlptr;
    bool start;
    bool quit;
    bool pause;
    bool back;
    bool end;
    bool view;
} ui;
void ui_pauseini();
void ui_control();
void ui_sync();
void ui_prnt();
void ui_insert();

struct user
{
    int leaderboard[5];
    int startlevel;
    int place;
} default_user;
void user_ini();
void user_newhigh();

struct sys_input
{
    int movecd;
    bool spacecd;
    bool rotatecd;
    bool holdcd;
    bool key_temp[2]; // 0 - a/left, 1 - d/right
} sys_input;
void sys_ini();
void sys_control();

struct hud
{
    int after[side_width][board_height - 6];
    int before[side_width][board_height/2];
}hud;
void hud_ini();
void hud_nextblock(int h_offset, int list_i);
void hud_hold();
int hud_scoreexceed();
char hud_inttochar();
void hud_sideclr(); //0 - all, 1 - after, 2 - before

struct blocks
{
    int type;
    int speedup;
    int pos[4][2];
    int ghost[4][2];
    int fixed[board_width][board_height];
    int list[2][7];
    int listptr;
    int hold;
    int gravity_time;
    int gravity_trigger;
    int fix_countdown;
    int move_countdown;
    int are;
    int level;
    int lineclr;
    int llineclr;
    int score;
    int baseline;
    bool fix;
    bool holded;
    bool touch[4]; //0 - left, 1 - up, 2 - right, 3 - down
} blocks;
const int block_list[7][4][2] = {{{0, 0}, {0, -1}, {1, 0}, {-1, 0}}, //0 - upside down T
                                {{0, 0}, {1, 0}, {1, -1}, {-1, 0}}, //1 - lay down L
                                {{0, 0}, {-1, 0}, {-1, -1}, {1, 0}}, //2 - inverted L
                                {{0, 0}, {1, 0}, {-1, -1}, {0, -1}}, //3 - Z
                                {{0, 0}, {-1, 0}, {1, -1}, {0, -1}}, //4 - inverted Z
                                {{0, 0}, {1, 0}, {-1, 0}, {-2, 0}}, //5 - line
                                {{0, 0}, {-1, 0}, {-1, -1}, {0, -1}}, //6 - square
                                };
void block_ini();
void block_gravity();
void block_spawn();
void block_listgen();
void block_listshift();
void block_spawnnext();
int block_bounds(); //return: 0 - w/in bounds except case -1&-2, -1 - touch left, -2 - touch right, 1 - 4 - units left bounds, 6 - 9 - units right bounds
void block_touch();
void block_move();
void block_rotate();
void block_wallkick();
void block_clear();
void block_ghost();
void block_hold();
void block_ARE();
void block_runARE();
void block_leveling();
void block_leveltogravity();
void block_scoring();
bool block_overlapped();
bool block_boundperblock();
bool block_over();

//------------------------------------------------//

void win_size()
{
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);

    SMALL_RECT winsize = {0, 0, 60, 24};
    SetConsoleWindowInfo(hOut, 1, &winsize);
}

//------------------------------------------------//

int main()
{
    srand(time(NULL));

    //Main Menu
    win_size();
    CDReset();
menu:
    UIInitial();
    while (!ui.start)
    {
        Menus();
        Render();

        if (ui.quit) return 0;

        Sleep((double) 1000/fps);
    }

    //Gameplay
    GameInitial();
    while(!over)
    {
        EarlyUpdate();
        Update();
        LateUpdate();

        if (ui.pause)
        {
            PauseIni();
            while (ui.pause)
            {
                Pause();
                Render();

                Sleep((double) 1000/fps);

                if (ui.back)
                {
                    goto menu;
                }
            }
        }

        Sleep((double)1000/fps);
    }

    //EndScreen
    Finish();
    while (ui.end)
    {
        End();
        Render();

        Sleep((double) 1000/fps);

        if (ui.back)
        {
            goto menu;
        }
        else if (ui.view)
        {
            while (ui.view)
            {
                ui_control();
                scrn_prnt();

                Sleep((double) 1000/fps);
            }
        }
    }

    return 0;
}

//------------------------------------------------------------//

void CDReset()
{
    for (int i = 0; i < 4; i++)
    {
        ui.lastcd[i] = 0;
    }
}

void UIInitial()
{
    ui.menus = 0;
    ui.main_selection = 0;
    ui.start = false;
    ui.quit = false;
    ui.pause = false;
    for (int i = 0; i < 4; i++)
    {
        ui.startlevels[i] = 5 * i;
    }
    ui.startlevels[4] = 18;
    ui.lvlptr = 0;
    default_user.startlevel = 0;
}

void PauseIni()
{
    ui_pauseini();
}

void Menus()
{
    ui_control();
}

void Render()
{
    ui_sync();
    ui_prnt();
}

void Pause()
{
    ui_control();
}

void End()
{
    ui_control();
}

//------------------------------------------------//

void ui_pauseini()
{
    ui.back = false;
    ui.menus = 3;
    ui.pause_selection = 0;
}

void ui_sync()
{
    for (int i = 0; i < board_height + 2; i++)
    {
        for (int j = 0; j < board_width + 4 + side_width * 2; j++)
        {
            ui.scrn[j][i] = 0;
        }
    }

    if (ui.menus == 0 || ui.menus == 1)
    {
        int logo[] = {  1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1,
                        0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0,
                        0, 1, 0, 0, 1, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 0, 1, 0,
                        0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1,
                        0, 1, 0, 0, 1, 1, 1, 0, 1, 0, 0, 1, 0, 1, 0, 1, 0, 1, 1, 1, 0 };

        ui_insert(4, 3, logo, 20, 5);
    }
    else if (ui.menus == 2)
    {
        int controls[] = {  0, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 0, 0, 0, 1, 1, 1,
                            1, 0, 0, 0, 0, 1, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0,
                            1, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 1, 0,
                            1, 0, 0, 0, 0, 1, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1,
                            0, 1, 1, 0, 0, 1, 0, 0, 1, 0, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0 };

        ui_insert(4, 3, controls, 19, 5);

        ui.scrn[2][12] = 8;
        ui.scrn[2][14] = 9;
        ui.scrn[2][16] = 10;
        for (int w = 1; w < 24; w++)
        {
            for (int h = 0; h < 3; h++)
            {
                ui.scrn[2 + w][12 + h * 2] = -1;
            }
        }
    }
    else if (ui.menus == 3)
    {
        int pause[] = { 1, 1, 0, 0, 0, 1, 0, 0, 1, 0, 1, 0, 1, 1, 1, 0, 1, 1, 1,
                        1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0,
                        1, 1, 0, 0, 1, 1, 1, 0, 1, 0, 1, 0, 0, 1, 0, 0, 1, 1, 0,
                        1, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0,
                        1, 0, 0, 0, 1, 0, 1, 0, 0, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0 };

        ui_insert(4, 3, pause, 19, 5);

        ui.scrn[11][12] = 14;
        ui.scrn[11][14] = 15;
        ui.scrn[11][16] = 16;
        for (int w = 1; w < 6; w++)
        {
            for (int h = 0; h < 3; h++)
            {
                ui.scrn[11 + w][12 + h * 2] = -1;
                if (h == ui.pause_selection)
                {
                    ui.scrn[10][12 + h * 2] = 5;
                    ui.scrn[17][12 + h * 2] = 6;
                }
            }
        }
    }
    else if (ui.menus == 4)
    {
        int gameOver[] = {  0, 1, 1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 1, 1, 1,
                            1, 0, 0, 0, 0, 1, 0, 1, 0, 1, 1, 0, 1, 1, 0, 1, 0, 0,
                            1, 0, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 1, 0,
                            1, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0,
                            0, 1, 1, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1, 0, 1, 1, 1,
                            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                            0, 0, 1, 1, 0, 0, 1, 0, 1, 0, 1, 1, 1, 0, 1, 1, 0, 0,
                            0, 1, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1, 0, 1, 0,
                            0, 1, 0, 0, 1, 0, 1, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 0,
                            0, 1, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1, 0, 1, 0,
                            0, 0, 1, 1, 0, 0, 0, 1, 0, 0, 1, 1, 1, 0, 1, 0, 1, 0, 0};

        ui_insert(5, 2, gameOver, 18, 11);

        ui.scrn[9][15] = 17;
        ui.scrn[9][17] = 18;
        for (int i = 1; i < 10; i++)
        {
            ui.scrn[9 + i][15] = -1;
            ui.scrn[9 + i][17] = -1;
        }

        ui.scrn[6][19] = 19;
        ui.scrn[16][19] = 20;
        for (int i = 1; i < 6; i++)
        {
            ui.scrn[6 + i][19] = -1;
            ui.scrn[16 + i][19] = -1;
        }

        ui.scrn[5 + ui.end_selection * 10][19] = 5;
        ui.scrn[12 + ui.end_selection * 10][19] = 6;
    }

    if (ui.menus == 0)
    {
        for (int j = 0; j < 3; j++)
        {
            if (j == ui.main_selection)
            {
                ui.scrn[10][12 + j * 2] = 5;
                ui.scrn[17][12 + j * 2] = 6;
            }

            ui.scrn[11][12 + j * 2] = 2 + j;
            for (int i = 1; i < 6; i++)
            {
                ui.scrn[11 + i][12 + j * 2] = -1;
            }
        }
    }
    else if (ui.menus == 1 || ui.menus == 2)
    {
        ui.scrn[0][board_height + 1] = 7;
        for (int i = 1; i < 13; i++)
        {
            ui.scrn[0 + i][board_height + 1] = -1;
        }
    }

    if (ui.menus == 1)
    {
        ui.scrn[10][12] = 11;
        for (int w = 1; w < 7; w++)
        {
            ui.scrn[10 + w][12] = -1;
        }

        ui.scrn[11][14] = 12;
        for (int w = 1; w < 5; w++)
        {
            ui.scrn[11 + w][14] = -1;
        }

        ui.scrn[5][16] = 13;
        for (int w = 1; w < 16; w++)
        {
            ui.scrn[5 + w][16] = -1;
        }
    }
}

void ui_prnt()
{
    unsigned char scrn[((side_width * 2 + board_width) * 2 + 1 + 4 + 2) * (board_height + 2 + 2) + 1] = {0};
    unsigned const char block[2] = {219, 219};

    //Top
    strcat (scrn, "\n");
    strncat (scrn, margin[0], 1);
    for (int i = 0; i < board_width * 2 + 4 + side_width * 4; i++)
    {
        strncat (scrn, margin[1], 1);
    }
    strncat (scrn, margin[2], 1);
    strcat (scrn, "\n");

    //Main
    for (int h = 0; h < board_height + 2; h++)
    {
        strncat (scrn, margin[3], 1);

        for (int w = 0; w < side_width * 2 + board_width + 2; w++)
        {
            if (ui.scrn[w][h] == 0)
            {
                strcat (scrn, "  ");
            }
            else if (ui.scrn[w][h] == 1)
            {
                strncat (scrn, block, 2);
            }
            else if (ui.scrn[w][h] == 2)
            {
                if (ui.main_selection == 0)
                {
                    strcat (scrn, "----PLAY----");
                }
                else
                {
                    strcat (scrn, "    PLAY    ");
                }
            }
            else if (ui.scrn[w][h] == 3)
            {
                if (ui.main_selection == 1)
                {
                    strcat (scrn, "--CONTROLS--");
                }
                else
                {
                    strcat (scrn, "  CONTROLS  ");
                }
            }
            else if (ui.scrn[w][h] == 4)
            {
                if (ui.main_selection == 2)
                {
                    strcat (scrn, "----QUIT----");
                }
                else
                {
                    strcat (scrn, "    QUIT    ");
                }
            }
            else if (ui.scrn[w][h] == 5)
            {
                strcat (scrn, " >");
            }
            else if (ui.scrn[w][h] == 6)
            {
                strcat (scrn, "< ");
            }
            else if (ui.scrn[w][h] == 7)
            {
                strcat (scrn, " ESC - return to main menu");
            }
            else if (ui.scrn[w][h] == 8)
            {
                strcat (scrn, "LEFT/A  - move left          DOWN/S - soft drop ");
            }
            else if (ui.scrn[w][h] == 9)
            {
                strcat (scrn, "RIGHT/D - move right         SPACE - hard drop  ");
            }
            else if (ui.scrn[w][h] == 10)
            {
                strcat (scrn, "UP/W    - rotate clockwise   C - Hold           ");
            }
            else if (ui.scrn[w][h] == 11)
            {
                strcat (scrn, "STARTING LEVEL");
            }
            else if (ui.scrn[w][h] == 12)
            {
                unsigned char temp[] = "LEVEL - 00";
                temp[8] = hud_inttochar(ui.startlevels[ui.lvlptr], 1, false);
                temp[9] = hud_inttochar(ui.startlevels[ui.lvlptr], 0, false);
                strncat (scrn, temp, 10);
            }
            else if (ui.scrn[w][h] == 13)
            {
                strcat (scrn, " *USE LEFT/RIGHT ARROW TO SELECT");
            }
            else if (ui.scrn[w][h] == 14)
            {
                if (ui.pause_selection == 0)
                {
                    strcat (scrn, "---Resume---");
                }
                else
                {
                    strcat (scrn, "   Resume   ");
                }
            }
            else if (ui.scrn[w][h] == 15)
            {
                if (ui.pause_selection == 1)
                {
                    strcat (scrn, "--CONTROLS--");
                }
                else
                {
                    strcat (scrn, "  CONTROLS  ");
                }
            }
            else if (ui.scrn[w][h] == 16)
            {
                strcat (scrn, "BACK TO MENU");
            }
            else if (ui.scrn[w][h] == 17)
            {
                strcat (scrn, "FINAL SCORE - ");

                char temp[6];

                int n = hud_scoreexceed(5);
                if (!n) temp[0] = hud_inttochar(blocks.score, 5, false);
                else    temp[0] = hud_inttochar(blocks.score, 5, true);

                for (int i = 1; i <= 5; i++)
                {
                    temp[i] = hud_inttochar(blocks.score, 5 - i, false);
                }

                strncat (scrn, temp, 6);
            }
            else if (ui.scrn[w][h] == 18)
            {
                if (default_user.place != 0)
                {
                    strcat (scrn, "(YOU GOT ");

                    if      (default_user.place == 1) strcat (scrn, "1ST");
                    else if (default_user.place == 2) strcat (scrn, "2ND");
                    else if (default_user.place == 3) strcat (scrn, "3RD");
                    else if (default_user.place == 4) strcat (scrn, "4TH");
                    else if (default_user.place == 5) strcat (scrn, "5TH");

                    strcat (scrn, " PLACE!)");
                }
                else
                {
                    strcat (scrn, "                    ");
                }
            }
            else if (ui.scrn[w][h] == 19)
            {
                if (ui.end_selection == 0)
                {
                    strcat (scrn, "-VIEW BOARD-");
                }
                else
                {
                    strcat (scrn, " VIEW BOARD ");
                }
            }
            else if (ui.scrn[w][h] == 20)
            {
                strcat (scrn, "BACK TO MENU");
            }
        }

        strncat (scrn, margin[3], 1);
        strcat (scrn, "\n");
    }

    //Bottom
    strncat (scrn, margin[4], 1);
    for (int i = 0; i < board_width * 2 + 4 + side_width * 4; i++)
    {
        strncat (scrn, margin[1], 1);
    }
    strncat (scrn, margin[5], 1);
    strcat (scrn, "\0");

    puts(scrn);
}

void ui_insert(int w, int h, int pic[], int sw, int sh)
{
    for (int i = 0, j = 0; j < sh; i++)
    {
        if (i == sw)
        {
            i = 0;
            j++;
        }

        ui.scrn[w + i][h + j] = pic[sw * j + i];
    }
}


void ui_control()
{
    if (GetAsyncKeyState(VK_UP))
    {
        if (ui.lastcd[0] == 0)
        {
            if (ui.menus == 0 && ui.main_selection > 0)
            {
                ui.main_selection--;
            }
            else if (ui.menus == 3 && ui.pause_selection > 0)
            {
                ui.pause_selection--;
            }
            ui.lastcd[0] = 1;
        }
    }
    else
    {
        ui.lastcd[0] = 0;
    }

    if (GetAsyncKeyState(VK_DOWN))
    {
        if (ui.lastcd[1] == 0)
        {
            if (ui.menus == 0 && ui.main_selection < 2)
            {
                ui.main_selection++;
            }
            else if (ui.menus == 3 && ui.pause_selection < 2)
            {
                ui.pause_selection++;
            }
            ui.lastcd[1] = 1;
        }
    }
    else
    {
        ui.lastcd[1] = 0;
    }

    if (GetAsyncKeyState(VK_RETURN))
    {
        if (ui.lastcd[2] == 0)
        {
            if (ui.menus == 0 && ui.main_selection == 2)
            {
                ui.quit = true;
            }
            else if (ui.menus == 0 && ui.main_selection == 0)
            {
                ui.menus = 1;
            }
            else if (ui.menus == 0 && ui.main_selection == 1)
            {
                ui.menus = 2;
            }
            else if (ui.menus == 1)
            {
                ui.start = true;
            }
            if (ui.menus == 3 && ui.pause_selection == 2)
            {
                ui.back = true;
            }
            else if (ui.menus == 3 && ui.pause_selection == 0)
            {
                ui.pause = false;
            }
            else if (ui.menus == 3 && ui.pause_selection == 1)
            {
                ui.menus = 2;
            }
            else if (ui.menus == 4 && ui.end_selection == 1)
            {
                ui.back = true;
            }
            else if (ui.menus == 4 && ui.end_selection == 0)
            {
                ui.view = true;
            }
            ui.lastcd[2] = 1;
        }
    }
    else
    {
        ui.lastcd[2] = 0;
    }

    if (GetAsyncKeyState(VK_ESCAPE))
    {
        if (ui.lastcd[3] == 0)
        {
            if (ui.menus == 0)
            {
                ui.quit = true;
            }
            else if (ui.menus == 1 || ui.menus == 2 && ui.pause == false)
            {
                ui.menus = 0;
            }
            else if (ui.menus == 3)
            {
                ui.pause = false;
            }
            else if (ui.menus == 2 && ui.pause == true)
            {
                ui.menus = 3;
            }
            else if (ui.menus == 4 && ui.view == true)
            {
                ui.view = false;
            }
            ui.lastcd[3] = 1;
        }
    }
    else
    {
        ui.lastcd[3] = 0;
    }

    if (GetAsyncKeyState(VK_LEFT))
    {
        if (ui.lastcd[4] == 0)
        {
            if (ui.menus == 1 && ui.lvlptr != 0)
            {
                ui.lvlptr--;
            }
            else if (ui.menus == 4 && ui.end_selection == 1)
            {
                ui.end_selection--;
            }
        }
        ui.lastcd[4] = 1;
    }
    else
    {
        ui.lastcd[4] = 0;
    }

    if (GetAsyncKeyState(VK_RIGHT))
    {
        if (ui.lastcd[5] == 0)
        {
            if (ui.menus == 1 && ui.lvlptr != 4)
            {
                ui.lvlptr++;
            }
            else if (ui.menus == 4 && ui.end_selection == 0)
            {
                ui.end_selection++;
            }
        }
        ui.lastcd[5] = 1;
    }
    else
    {
        ui.lastcd[5] = 0;
    }
}


//------------------------------------------------//

void GameInitial()
{
    sys_ini();
    user_ini();
    block_ini();
    hud_ini();
    block_spawn(5, 1, blocks.list[0][blocks.listptr++]);
    over = false;
    ui.pause = false;
}

void EarlyUpdate()
{
    block_touch();
    block_clear();
}

void Update()
{
    if (!blocks.fix)
    {
        sys_control();
        block_gravity();
    }
    else
    {
        if (blocks.holded)
        {
            blocks.holded = false;
        }

        block_spawnnext();
    }
}

void LateUpdate()
{
    block_ghost();
    board_sync();
    scrn_prnt();
    block_runARE();
}

void Finish()
{
    user_newhigh();
    ui.end = true;
    ui.menus = 4;
    ui.end_selection = 0;
    ui.back = false;
    ui.view = false;
}

//-------------------------------------------------//

void board_sync()
{
    for (int h = 0; h < board_height; h++)
    {
        for (int w = 0; w < board_width; w++)
        {
            board[w][h] = 0;
        }
    }

    if (blocks.are <= 0)
    {
        for (int j = 0; j < 4; j++)
        {
            if (block_boundperblock(blocks.ghost[j][0], blocks.ghost[j][1]))
            {
                board[blocks.ghost[j][0]][blocks.ghost[j][1]] = 2;
            }
        }

        for (int j = 0; j < 4; j++)
        {
            if (block_boundperblock(blocks.pos[j][0], blocks.pos[j][1]))
            {
                board[blocks.pos[j][0]][blocks.pos[j][1]] = 1;
            }
        }
    }

    for (int i = 0; i < board_width; i++)
    {
        for (int j = 0; j < board_height; j++)
        {
            if (blocks.fixed[i][j] == 1)
            {
                board[i][j] = 1;
            }
        }
    }
}

//---------------------------------------------------------------//

void scrn_prnt()
{
    unsigned char scrn[(board_width * 2 + 4 + side_width * 4 + 1 + 2) * (board_height + 4) + 2 + 500] = {0};
    unsigned const char block[2] = {219, 219};
    unsigned const char ghost[2] = {176, 176};

    strcat (scrn, "\n");

    //Top
    strncat (scrn, margin[0], 1);
    for (int i = 0; i < board_width * 2 + 4 + side_width * 4; i++)
    {
        strncat (scrn, margin[1], 1);
    }
    strncat (scrn, margin[2], 1);
    strcat (scrn, "\n");

    //Main
    for (int h = 0; h < board_height; h++)
    {
        strncat (scrn, margin[3], 1);

        if (h < board_height/2)
        {
            for (int w = 0; w < side_width; w++)
            {
                if (hud.before[w][h] == 0)
                {
                    strcat (scrn, "  ");
                }
                else if (hud.before[w][h] == 1)
                {
                    strncat (scrn, block, 2);
                }
                else if (hud.before[w][h] == 2)
                {
                    strcat (scrn, "HOLD");
                }
            }
        }
        else
        {
            if (h == board_height/2 + 1)
            {
                strcat (scrn, " SCORE:         ");
            }
            else if (h == board_height/2 + 2)
            {
                strcat (scrn, " ");

                char temp[6];

                int n = hud_scoreexceed(5);
                if (!n) temp[0] = hud_inttochar(blocks.score, 5, false);
                else    temp[0] = hud_inttochar(blocks.score, 5, true);

                for (int i = 1; i <= 5; i++)
                {
                    temp[i] = hud_inttochar(blocks.score, 5 - i, false);
                }

                strncat (scrn, temp, 6);
                strcat (scrn, "         ");
            }
            else if (h == board_height/2 + 4)
            {
                strcat (scrn, " LEVEL:         ");
            }
            else if (h == board_height/2 + 5)
            {
                strcat (scrn, " ");

                char temp[2];
                temp[0] = hud_inttochar(blocks.level, 1, false);
                temp[1] = hud_inttochar(blocks.level, 0, false);

                strncat (scrn, temp, 2);
                strcat (scrn, "             ");
            }
            else if (h == board_height/2 + 7)
            {
                strcat (scrn, " LINE CLEARED:  ");
            }
            else if (h == board_height/2 + 8)
            {
                strcat (scrn, " ");

                char temp[3];
                temp[0] = hud_inttochar(blocks.lineclr, 2, false);
                temp[1] = hud_inttochar(blocks.lineclr, 1, false);
                temp[2] = hud_inttochar(blocks.lineclr, 0, false);

                strncat (scrn, temp, 3);
                strcat (scrn, "            ");
            }
            else
            {
                strcat (scrn, "                ");
            }
        }

        strcat (scrn, "<!");

        for (int w = 0; w < board_width; w++)
        {
            if (board[w][h] == 0)
            {
                strcat (scrn, " .");
            }
            else if (board[w][h] == 1)
            {
                strncat (scrn, block, 2);
            }
            else if (board[w][h] == 2)
            {
                strncat(scrn, ghost, 2);
            }
        }
        strcat (scrn, "!>");

        if (h < board_height - 6)
        {
            for (int w = 0; w < side_width; w++)
            {
                if (hud.after[w][h] == 0)
                {
                    strcat (scrn, "  ");
                }
                else if (hud.after[w][h] == 1)
                {
                    strncat (scrn, block, 2);
                }
                else if (hud.after[w][h] == 2)
                {
                    strcat (scrn, "NEXT");
                }
            }
        }
        else
        {
            if (h == board_height - 6)
            {
                strcat (scrn, " LEADERBOARD:   ");
            }
            else
            {
                int i = 5 - (board_height - h);

                strcat (scrn, " ");

                char tempn[1];
                tempn[0] = i + 1 + '0';
                strncat (scrn, tempn, 1);

                strcat (scrn, ". ");

                char temp[6];

                int n = hud_scoreexceed(i);
                if (!n) temp[0] = hud_inttochar(default_user.leaderboard[i], 5, false);
                else    temp[0] = hud_inttochar(default_user.leaderboard[i], 5, true);

                for (int j = 1; j <= 5; j++)
                {
                    temp[j] = hud_inttochar(default_user.leaderboard[i], 5 - j, false);
                }

                strncat (scrn, temp, 6);
                strcat (scrn, "      ");
            }
        }

        strncat (scrn, margin[3], 1);
        strcat (scrn, "\n");
    }
    strncat (scrn, margin[3], 1);
    strcat (scrn, "                ");
    strcat (scrn, "<!====================!>");
    strcat (scrn, "                ");
    strncat (scrn, margin[3], 1);
    strcat (scrn, "\n");
    strncat (scrn, margin[3], 1);
    if (ui.view && ui.menus == 4)
    {
        strcat (scrn, " ESC - go back  ");
    }
    else
    {
        strcat (scrn, "                ");
    }
    strcat (scrn, "  \\/\\/\\/\\/\\/\\/\\/\\/\\/\\/  ");
    strcat (scrn, "                ");
    strncat (scrn, margin[3], 1);
    strcat (scrn, "\n");

    //Bottom
    strncat (scrn, margin[4], 1);
    for (int i = 0; i < board_width * 2 + 4 + side_width * 4; i++)
    {
        strncat (scrn, margin[1], 1);
    }
    strncat (scrn, margin[5], 1);
    strcat (scrn, "\0");

    puts(scrn);
}

//----------------------------------------------------------------------//

void user_ini()
{
    default_user.startlevel = ui.startlevels[ui.lvlptr];

    FILE* infile = fopen("leaderboard", "r");

    if (infile == NULL)
    {
        fclose(infile);
        infile = fopen("leaderboard", "w");

        for (int i = 0; i < 5; i++)
        {
            fprintf(infile, "000000\n");
        }

        fclose(infile);
    }

    infile = fopen("leaderboard", "r");
    char sixth;
    for (int i = 0; i < 5; i++)
    {
        fscanf(infile, "%c%d\n", &sixth, &default_user.leaderboard[i]);
        if (sixth >= 'A')
        {
            sixth -= 'A' - 10;
        }
        else
        {
            sixth -= '0';
        }
        default_user.leaderboard[i] += sixth * 100000;
    }
    fclose(infile);
}

void user_newhigh()
{
    default_user.place = 0;
    if (blocks.score > default_user.leaderboard[4])
    {
        char output[7 * 5] = {0};

        for (int i = 3; i >= 0; i--)
        {
            if (blocks.score < default_user.leaderboard[i] || blocks.score > default_user.leaderboard[i] && i == 0)
            {
                bool flag = false;

                if (blocks.score > default_user.leaderboard[i] && i == 0)
                {
                    i = -1;
                    flag = true;
                }

                default_user.place = i + 1 + 1;

                for (int j = 0 ; j < 5; j++)
                {
                    if (j == i + 1)
                    {
                        flag = true;
                        char temp[6];

                        int n = hud_scoreexceed(5);
                        if (!n) temp[0] = hud_inttochar(blocks.score, 5, false);
                        else    temp[0] = hud_inttochar(blocks.score, 5, true);

                        for (int j = 1; j <= 5; j++)
                        {
                            temp[j] = hud_inttochar(blocks.score, 5 - j, false);
                        }

                        strncat (output, temp, 6);
                    }
                    else
                    {
                        int k = j - 1 * flag;
                        char temp[6];

                        int n = hud_scoreexceed(k);
                        if (!n) temp[0] = hud_inttochar(default_user.leaderboard[k], 5, false);
                        else    temp[0] = hud_inttochar(default_user.leaderboard[k], 5, true);

                        for (int j = 1; j <= 5; j++)
                        {
                            temp[j] = hud_inttochar(default_user.leaderboard[k], 5 - j, false);
                        }

                        strncat (output, temp, 6);
                    }

                    strcat (output, "\n");
                }

                break;
            }
        }

        FILE* outfile = fopen("leaderboard", "w");
        fprintf(outfile, "%s", output);
        fclose(outfile);
    }
}

//----------------------------------------------------------------------//

void sys_ini()
{
    sys_input.spacecd = 0;
    sys_input.movecd = 0;
    sys_input.rotatecd = 0;
    sys_input.holdcd = 0;
}

void sys_control()
{
    if (GetAsyncKeyState(0x53) || GetAsyncKeyState(VK_DOWN))
    {
        blocks.speedup = 1;
    }
    else
    {
        blocks.speedup = 0;
    }

    if (GetAsyncKeyState(0x41) || GetAsyncKeyState(VK_LEFT))
    {
        if (!sys_input.key_temp[0])
        {
            sys_input.movecd = 0;
            sys_input.key_temp[0]++;
        }

        if (sys_input.movecd++ <= 0 || sys_input.movecd > 8)
        {
            block_move(97);
        }
    }
    else if (GetAsyncKeyState(0x44) || GetAsyncKeyState(VK_RIGHT))
    {
        if (!sys_input.key_temp[1])
        {
            sys_input.movecd = 0;
            sys_input.key_temp[1]++;
        }

        if (sys_input.movecd++ <= 0 || sys_input.movecd > 8)
        {
            block_move(100);
        }
    }
    if (!(GetAsyncKeyState(0x41) || GetAsyncKeyState(VK_LEFT)))
    {
        sys_input.key_temp[0] = 0;
    }
    if (!(GetAsyncKeyState(0x44) || GetAsyncKeyState(VK_RIGHT)))
    {
        sys_input.key_temp[1] = 0;
    }

    if ((GetAsyncKeyState(0x57) || GetAsyncKeyState(VK_UP)) && sys_input.rotatecd)
    {
        sys_input.rotatecd = 0;
        block_rotate();
    }
    else if (!(GetAsyncKeyState(0x57) || GetAsyncKeyState(VK_UP)) && !sys_input.rotatecd)
    {
        sys_input.rotatecd = 1;
    }

    if (GetAsyncKeyState(0x43) && sys_input.holdcd)
    {
        sys_input.holdcd = 0;
        block_hold();
        hud_sideclr(2);
        hud_hold();
    }
    else if (!GetAsyncKeyState(0x43) && !sys_input.holdcd)
    {
        sys_input.holdcd = 1;
    }


    if (GetAsyncKeyState(VK_SPACE) && sys_input.spacecd)
    {
        sys_input.spacecd = 0;
        blocks.fix = true;
        int height = 0;
        for (int i = 0; i < 4; i++)
        {
            if (blocks.ghost[i][1] >= 0)
            {
                blocks.fixed[blocks.ghost[i][0]][blocks.ghost[i][1]] = 1;
            }
            else
            {
                over = true;
            }

            if (blocks.pos[i][1] > height)
            {
                height = blocks.pos[i][1];
            }
        }

        block_ARE(height);
    }
    else if (!GetAsyncKeyState(VK_SPACE) && !sys_input.spacecd)
    {
        sys_input.spacecd = 1;
    }

    if (GetAsyncKeyState(VK_ESCAPE))
    {
        if (ui.lastcd[3] == 0)
        {
            ui.pause = true;
        }
        ui.lastcd[3] = 1;
    }
    else
    {
        ui.lastcd[3] = 0;
        ui.pause = false;
    }
}

//----------------------------------------------------------------------//

void hud_ini()
{
    for (int w = 0; w < side_width; w++)
    {
        hud.before[w][0] = 0;
        hud.after[w][0] = 0;
    }
    hud_sideclr(0);

    hud.after[3][0] = 2;
    hud.after[4][0] = -1;

    hud.before[3][0] = 2;
    hud.before[4][0] = -1;

    for (int i = 1; i <= 3; i++)
    {
        hud_nextblock(i * 4, i);
    }
}

void hud_nextblock(int h_offset, int list_i)
{
    if (blocks.listptr + list_i > 6)
    {
        for (int i = 0; i < 4; i++)
        {
            hud.after[4 + block_list[blocks.list[1][(blocks.listptr + list_i) % 7]][i][0]][h_offset - 1 + block_list[blocks.list[1][(blocks.listptr + list_i) % 7]][i][1]] = 1;
        }
    }
    else
    {
        for (int i = 0; i < 4; i++)
        {
            hud.after[4 + block_list[blocks.list[0][blocks.listptr + list_i]][i][0]][h_offset - 1 + block_list[blocks.list[0][blocks.listptr + list_i]][i][1]] = 1;
        }
    }
}

void hud_hold()
{
    if (blocks.hold != -1)
    {
        for (int i = 0; i < 4; i++)
        {
            hud.before[4 + block_list[blocks.hold][i][0]][3 + block_list[blocks.hold][i][1]] = 1;
        }
    }
}

int hud_scoreexceed(int n)
{
    int i;
    if (n == 5)
    {
        for (i = 0; (int) (blocks.score / (pow(10, i + 6))) != 0; i++)
            continue;
    }
    else
    {
        for (i = 0; (int) (default_user.leaderboard[n] / (pow(10, i + 6))) != 0; i++)
            continue;
    }
    return i;
}

char hud_inttochar(int n, int ptr, int x)
{
    if (!x)
    {
        return  ((int)(floor(n / pow(10, ptr))) % 10) + '0';
    }
    else
    {
        return (floor(n / pow(10, ptr)) - 10) + 'A';
    }
}

void hud_sideclr(int sec)
{
    for (int h = 1; h < board_height - 6; h++)
    {
        for (int w = 0; w < side_width; w++)
        {
            if (sec == 0 || sec == 1)
            {
                hud.after[w][h] = 0;
            }

            if ((sec == 0 || sec == 2) && h < board_height/2)
            {
                hud.before[w][h] = 0;
            }
        }
    }
}

//----------------------------------------------------------------------//

void block_ini()
{
    blocks.listptr = 0;
    blocks.gravity_time = 0;
    blocks.gravity_trigger = 48;
    blocks.fix = false;
    blocks.fix_countdown = -1;
    blocks.hold = -1;
    blocks.holded = false;
    blocks.are = -1;
    blocks.level = default_user.startlevel;
    blocks.lineclr = 0;
    blocks.llineclr = 0;
    blocks.score = 0;
    block_listgen();
    block_listshift();
    block_listgen();
    block_ghost();

    if (blocks.list[0][6] == blocks.list[1][0])
    {
        block_listgen();
    }

    for (int h = 0; h < board_height; h++)
    {
        for (int w = 0; w < board_width; w++)
        {
            blocks.fixed[w][h] = 0;
        }
    }

    if (blocks.level * 10 + 10 < 100)
    {
        blocks.baseline = blocks.level * 10 + 10;
    }
    else if (blocks.level * 10 - 50 > 100)
    {
        blocks.baseline = blocks.level * 10 - 50;
    }
    else
    {
        blocks.baseline = 100;
    }

    block_leveltogravity();
}

void block_gravity()
{
    if (blocks.gravity_time >= blocks.gravity_trigger && blocks.fix_countdown == -1)
    {
        for (int i = 0; i < 4; i++)
        {
            if (blocks.pos[i][1] == 19 || blocks.touch[3])
            {
                blocks.fix_countdown = 16;
                g_fix = true;
            }
        }

        if (!g_fix)
        {
            for (int j = 0; j < 4; j++)
            {
                blocks.pos[j][1]++;
            }
        }

        blocks.gravity_time = 0;
    }
    else if (blocks.fix_countdown == -1)
    {
        blocks.gravity_time += 1 * (1 - blocks.speedup) + blocks.speedup * (int) blocks.gravity_trigger / 3;
    }
    else if (blocks.fix_countdown > 0 && blocks.fix_countdown <= 16 && blocks.touch[3])
    {
        blocks.fix_countdown--;
    }
    else if (blocks.fix_countdown <= 0 && blocks.touch[3])
    {
        int height = 0;
        for (int i = 0; i < 4; i++)
        {
            if (blocks.pos[i][1] >= 0)
            {
                blocks.fixed[blocks.pos[i][0]][blocks.pos[i][1]] = 1;
            }
            else
            {
                over = true;
            }
            if (blocks.pos[i][1] > height)
            {
                height = blocks.pos[i][1];
            }
        }
        block_ARE(height);
        blocks.fix_countdown = -1;
        blocks.fix = true;
        g_fix = false;
    }

    if (!blocks.touch[3])
    {
        blocks.fix_countdown = -1;
        g_fix = false;
    }
}

void block_spawn(int w, int h, int block_number)
{
    for (int i = 0; i < 4; i++)
    {
        blocks.pos[i][0] = w + block_list[block_number][i][0];
        blocks.pos[i][1] = h + block_list[block_number][i][1];
    }
    blocks.type = block_number;
    blocks.gravity_time = 0;
    blocks.fix_countdown = -1;
    blocks.fix = false;

    for (int i = 0; i < 4; i++)
    {
        bool changed = false;
        if (blocks.pos[i][1] >= 0)
        {
          while (blocks.fixed[blocks.pos[i][0]][blocks.pos[i][1]])
            {
                for (int j = 0; j < 4; j++)
                {
                    blocks.pos[j][1]--;
                }
                changed = true;
            }
        }

        if (changed)
        {
            i = 0;
        }
    }

    bool allout = true;
    for (int i = 0; i < 4; i++)
    {
        if (blocks.pos[i][1] >= 0)
        {
            allout = false;
            break;
        }
    }
    if (allout)
    {
        over = true;
    }
}

void block_listgen()
{
    for (int i = 0; i < 7; i++)
    {
        blocks.list[1][i] = i;
    }

    for (int i = 6; i > 0; i--)
    {
        int ptr = rand() % i;
        int temp = blocks.list[1][i];
        blocks.list[1][i] = blocks.list[1][ptr];
        blocks.list[1][ptr] = temp;
    }
}

void block_listshift()
{
    for (int i = 0; i < 7; i++)
    {
        blocks.list[0][i] = blocks.list[1][i];
    }
}

void block_spawnnext()
{
    block_spawn(5, 1, blocks.list[0][blocks.listptr++]);
    if (blocks.listptr >= 7)
    {
        blocks.listptr %= 7;
        block_listshift();
        block_listgen();
    }
    if (blocks.list[0][6] == blocks.list[1][0])
    {
        block_listgen();
    }

    hud_sideclr(1);
    for (int i = 1; i <= 3; i++)
    {
        hud_nextblock(i * 4, i - 1);
    }
}

int block_bounds()
{
    int y = 0;

    for (int i = 0; i < 4; i++)
    {
        if (blocks.pos[i][0] == 0 && y <= 0)
        {
            y = -1;
        }
        else if (blocks.pos[i][0] == 9 && y <= 0)
        {
            y = -2;
        }
        else if (blocks.pos[i][0] < 0)
        {
            y = 5 + blocks.pos[i][0];
        }
        else if (blocks.pos[i][0] > 9)
        {
            y = blocks.pos[i][0] - 4;
        }
    }

    return y;
}

void block_touch()
{
    for (int i = 0; i < 4; i++)
    {
        blocks.touch[i] = false;
    }

    for (int i = 0; i < 4; i++)
    {
        for (int j = 0, w = 0, h = 1; j < 4; j++)
        {
            int temp = w;
            w = h * -1;
            h = temp;
            int width = blocks.pos[i][0] + w;
            int height = blocks.pos[i][1] + h;
            if (width > 9 || width < 0 || height > 19 || height < 0)
            {
                continue;
            }

            if (blocks.fixed[width][height] == 1)
            {
                blocks.touch[j] = true;
            }
        }

        if (blocks.pos[i][1] == 19)
        {
            blocks.touch[3] = true;
        }
    }
}

void block_move(int input)
{
    if ((input == 97 && block_bounds() != -1 && !blocks.touch[0] || input == 100 && block_bounds() != -2 && !blocks.touch[2]))
    {
        for (int i = 0; i < 4; i++)
        {
            blocks.pos[i][0] += trunc(input - 98.5);
        }
    }
}

void block_rotate()
{
    int temp_block[4][2];
    for (int i = 0; i < 4; i++)
    {
        temp_block[i][0] = blocks.pos[i][0];
        temp_block[i][1] = blocks.pos[i][1];
    }

    int olow = 0;
    for (int i = 0; i < 4; i++)
    {
        if (blocks.pos[i][1] > olow)
        {
            olow = blocks.pos[i][1];
        }
    }

    if (blocks.type <= 5)
    {
        for (int i = 1; i < 4; i++)
        {
            int temp = blocks.pos[i][0];
            blocks.pos[i][0] = (blocks.pos[i][1] - blocks.pos[0][1]) * -1 + blocks.pos[0][0];
            blocks.pos[i][1] = (temp - blocks.pos[0][0]) + blocks.pos[0][1];
        }
    }

    if (blocks.type == 5)
    {
        int dir[2];
        dir[0] = blocks.pos[0][0] - blocks.pos[1][0];
        dir[1] = blocks.pos[0][1] - blocks.pos[1][1];

        for (int i = 0; i < 4; i++)
        {
            blocks.pos[i][0] -= dir[0];
            blocks.pos[i][1] -= dir[1];
        }
    }

    int nlow = 0;
    for (int i = 0; i < 4; i++)
    {
        if (blocks.pos[i][1] > nlow)
        {
            nlow = blocks.pos[i][1];
        }
    }

    int difflow = 0;
    if (olow > nlow)
    {
        difflow = olow - nlow;
    }

    if (block_overlapped())
    {
        for (int i = 0; i < 4; i++)
        {
            blocks.pos[i][0]++;
        }

        if (block_overlapped())
        {
            for (int i = 0; i < 4; i++)
            {
                blocks.pos[i][0] -= 2;
            }

            if (block_overlapped())
            {
                for (int i = 0; i < 4; i++)
                {
                    blocks.pos[i][0]++;
                }

                for (int i = 0; i < difflow; i++)
                {
                    for (int j = 0; j < 4; j++)
                    {
                        blocks.pos[j][1]++;
                    }

                    if (blocks.type == 5)
                    {
                        if (block_overlapped())
                        {
                            for (int i = 0; i < 4; i++)
                            {
                                blocks.pos[i][0] -= 2;
                            }

                            if (!block_overlapped())
                            {
                                break;
                            }
                            else
                            {
                                for (int i = 0; i < 4; i++)
                                {
                                    blocks.pos[i][0] += 4;
                                }

                                if (!block_overlapped())
                                {
                                    break;
                                }
                                else
                                {
                                    for (int i = 0; i < 4; i++)
                                    {
                                        blocks.pos[i][0]--;
                                    }
                                }
                            }
                        }
                        else
                        {
                            break;
                        }
                    }

                    if (block_overlapped())
                    {
                        for (int i = 0; i < 4; i++)
                        {
                            blocks.pos[i][0]--;
                        }

                        if (!block_overlapped())
                        {
                            break;
                        }
                        else
                        {
                            for (int i = 0; i < 4; i++)
                            {
                                blocks.pos[i][0] += 2;
                            }

                            if (!block_overlapped())
                            {
                                break;
                            }
                            else
                            {
                                for (int i = 0; i < 4; i++)
                                {
                                    blocks.pos[i][0]--;
                                }
                            }
                        }
                    }
                    else
                    {
                        break;
                    }
                }
            }
        }
    }

    bool safe = false;
    int count = 0;
    while (!safe)
        {
        if (block_overlapped())
        {
            for (int j = 0; j < 4; j++)
            {
                blocks.pos[j][1]--;
            }
        }
        else if (!block_overlapped())
        {
            safe = true;
        }
        count++;
        if (count == 3)
        {
            for (int i = 0; i < 4; i++)
            {
                blocks.pos[i][0] = temp_block[i][0];
                blocks.pos[i][1] = temp_block[i][1];
            }
            break;
        }
    }
}

void block_clear()
{
    int n = 0;

    for (int h = 0; h < board_height; h++)
    {
        bool fixed = true;
        for (int w = 0; w < board_width; w++)
        {
            if (blocks.fixed[w][h] == 0)
            {
                fixed = false;
                break;
            }
        }

        if (fixed)
        {
            n++;
            blocks.lineclr++;
            block_leveling();

            for (int i = h; i > 0; i--)
            {
                for (int j = 0; j < board_width; j++)
                {
                    blocks.fixed[j][i] = blocks.fixed[j][i - 1];
                }
            }

            for (int i = 0; i < board_width; i++)
            {
                blocks.fixed[i][0] = 0;
            }
        }
    }

    block_scoring(n);
}

void block_ghost()
{
    for (int i = 0; i < 4; i++)
    {
        blocks.ghost[i][0] = blocks.pos[i][0];
        blocks.ghost[i][1] = blocks.pos[i][1];
    }

    bool bottom_touch = false;
    while (!bottom_touch)
    {
        for (int i = 0; i < 4; i++)
        {
            if (blocks.fixed[blocks.ghost[i][0]][blocks.ghost[i][1] + 1] == 1 || blocks.ghost[i][1] == 19)
            {
                bottom_touch = true;
            }
        }

        if (!bottom_touch)
        {
            for (int i = 0; i < 4; i++)
            {
                blocks.ghost[i][1]++;
            }
        }
    }
}

void block_hold()
{
    if (!blocks.holded)
    {
        if (blocks.hold == -1)
        {
            blocks.hold = blocks.type;
            block_spawnnext();
        }
        else
        {
            int temp = blocks.type;
            block_spawn(5, 1, blocks.hold);
            blocks.hold = temp;
        }
    }
    blocks.holded = true;
}

void block_ARE(int height)
{
    if      (height == 19 || height == 18) blocks.are = 10;
    else if (height <= 17 && height >= 14) blocks.are = 12;
    else if (height <= 13 && height >= 10) blocks.are = 14;
    else if (height <= 9 && height >= 6)   blocks.are = 16;
    else if (height <= 5)                  blocks.are = 18;
}

void block_runARE()
{
    while (blocks.are > 0)
    {
        Sleep((double)1000/fps);
        blocks.are--;
    }
}

void block_leveling()
{
    if (blocks.level == default_user.startlevel)
    {
        int temp = blocks.lineclr - blocks.llineclr - blocks.baseline;
        if (temp >= 0)
        {
            blocks.llineclr = blocks.lineclr - temp;
            blocks.level++;
        }
    }
    else
    {
        int temp = blocks.lineclr - blocks.llineclr - 10;
        if (temp >= 0)
        {
            blocks.llineclr = blocks.lineclr - temp;
            blocks.level++;
        }
    }

    block_leveltogravity();
}

void block_leveltogravity()
{
    if      (blocks.level <= 8)                        blocks.gravity_trigger = 48 - 5 * blocks.level;
    else if (floor((blocks.level - 1) / 3) <= 5)       blocks.gravity_trigger = 8 - floor((blocks.level - 1) / 3);
    else if (blocks.level >= 19 && blocks.level <= 28) blocks.gravity_trigger = 2;
    else if (blocks.level >= 29)                       blocks.gravity_trigger = 0;
}

void block_scoring(int n)
{
    int pts;
    if      (n == 1) pts = 40;
    else if (n == 2) pts = 100;
    else if (n == 3) pts = 300;
    else if (n == 4) pts = 1200;
    else             pts = 0;

    blocks.score += pts * (blocks.level + 1);
}

bool block_overlapped()
{
    for (int i = 0; i < 4; i++)
    {
        if (blocks.pos[i][0] > 9 || blocks.pos[i][0] < 0 || blocks.pos[i][1] > 19)
        {
            return true;
        }
        else if (blocks.pos[i][0] <= 9 && blocks.pos[i][0] >= 0 && blocks.pos[i][1] <= 19)
        {
            if (blocks.fixed[blocks.pos[i][0]][blocks.pos[i][1]] == 1)
                return true;
        }
    }
    return false;
}

bool block_boundperblock(int x, int y)
{
    if (x > 9 || x < 0 || y > 19 || y < 0)
    {
        return false;
    }
    return true;
}
