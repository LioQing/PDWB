#include <stdio.h>
#include <stdbool.h>
#include <windows.h>
#include <conio.h>
#include <string.h>
#include <time.h>

#define MAX_TAIL_NO 2500
#define MAX_SIZE 60
#define MIN_SIZE 10
#define MAX_SPEED 10

void Initial();
void Start();
void FixedUpdate();
void LateUpdate();
void Finish();

void screen_render(int);
void screen_score();

void snake_movement_input(char input);
void snake_tail();
bool snake_isTail(int y, int x);
void snake_postMove();
void snake_ini();
void snake_spawn();
void snake_highscore_ini();
void snake_highscore_save();
struct snake
{
    int pos[2];
    int movement[2];
    int tail_pos[MAX_TAIL_NO][2];
    int tail_no;
    int score;
    int highscore;
} snake;

void board_sync();
int board[MAX_SIZE][MAX_SIZE] = {0};

void apple_spawn();
void apple_eat();
int apple[2];

int WIDTH = 20;
int HEIGHT = 20;
int speed = 7;
bool finish = false;
bool start = false;

HANDLE hOut;
COORD home_cursor_pos = { 0, 0 };

int main ()
{
    Initial();

    while (!finish)
    {
        if (!start)
        {
            Start();
            if (start)
            {
                system("cls");
                SMALL_RECT WINDOW_SIZE = {0, 0, (SHORT)(WIDTH * 2 + 11), (SHORT)(HEIGHT + 2)};
                SetConsoleWindowInfo(hOut, 1, &WINDOW_SIZE);
            }
        }
        else
        {
            FixedUpdate();
            LateUpdate();

            if (finish)
                Finish();
        }
    }

    return 0;
}

////////////////////////////////////////////////////////////////////////

void Initial()
{
    hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    SMALL_RECT WINDOW_SIZE = {0, 0, 600, 200};
    SetConsoleWindowInfo(hOut, 1, &WINDOW_SIZE);
    srand(time(NULL));
    snake_ini();
    snake_spawn();
    snake_highscore_ini();
}

void Start()
{
    int input;
    bool con = false;
    apple_spawn();

    while (!con)
    {
        bool subcon = false;
        con = true;

        printf("Settings (Enter number to configure):\n\n");
        printf("1. Grid Size:           %d x %d (Height x Width)\n", HEIGHT, WIDTH);
        printf("2. Speed:               %d\n", speed);
        printf("3. Initial Tail Length: %d\n\n", snake.tail_no);
        printf("Press ESC during the game to exit\n\n");
        printf("Enter 0 to start the game\n\n");

        scanf("%d", &input);
        fflush(stdin);
        printf("\n");

        if (input == 0)
        {
            start = true;
            return;
        }
        else if (input == 1)
        {
            subcon = false;
            while (!subcon)
            {
                subcon = true;
                printf("Enter height of grid size (%d - %d): ", MIN_SIZE, MAX_SIZE);
                scanf("%d", &HEIGHT);
                fflush(stdin);

                if (HEIGHT < MIN_SIZE || HEIGHT > MAX_SIZE)
                {
                    subcon = false;
                    printf("Please enter a valid number!\n\n");
                }
            }

            subcon = false;
            while (!subcon)
            {
                subcon = true;
                printf("Enter width of grid size (%d - %d): ", MIN_SIZE, MAX_SIZE);
                scanf("%d", &WIDTH);
                fflush(stdin);

                if (WIDTH < MIN_SIZE || WIDTH > MAX_SIZE)
                {
                    subcon = false;
                    printf("Please enter a valid number!\n\n");
                }
            }

            snake_spawn();
        }
        else if (input == 2)
        {
            subcon = false;
            while (!subcon)
            {
                subcon = true;
                printf("Enter speed (1 - %d): ", MAX_SPEED);
                scanf("%d", &speed);
                fflush(stdin);

                if (speed < 1 || speed > MAX_SPEED)
                {
                    subcon = false;
                    printf("Please enter a valid number!\n\n");
                }
            }
        }
        else if (input == 3)
        {
            subcon = false;
            while (!subcon)
            {
                subcon = true;
                printf("Enter initial tail length (0 - %d): ", MAX_TAIL_NO);
                scanf("%d", &snake.tail_no);
                fflush(stdin);

                if (snake.tail_no < 0 || snake.tail_no > MAX_TAIL_NO)
                {
                    subcon = false;
                    printf("Please enter a valid number!\n\n");
                }
            }
        }
        else
        {
            con = false;
            printf("Please enter a valid number!\n");
        }

        printf("\n");
    }
}

void FixedUpdate()
{
    char input;

    if (kbhit())
    {
        input = getch();

        if (input == 27)
        {
            finish = true;
            return;
        }
        else
            snake_movement_input(input);
    }

    snake_postMove();
    if (!finish)
        snake_tail();

    apple_eat();
}

void LateUpdate()
{
    Sleep(250 - (25 * (speed - 1)));

    board_sync();
    screen_render(0);
}

void Finish()
{
    screen_render(1);

    if (snake.score > snake.highscore)
    {
        snake_highscore_save();
    }
}

//////////////////////////////////////////////////////////////////////////

void snake_movement_input(char input)
{
    if ((input == 's' || input == 'S') && snake.movement[0] != -1)
    {
        snake.movement[0] = 1;
        snake.movement[1] = 0;
    }
    else if ((input == 'w' || input == 'W') && snake.movement[0] != 1)
    {
        snake.movement[0] = -1;
        snake.movement[1] = 0;
    }
    else if ((input == 'd' || input == 'D') && snake.movement[1] != -1)
    {
        snake.movement[0] = 0;
        snake.movement[1] = 1;
    }
    else if ((input == 'a' || input == 'A') && snake.movement[1] != 1)
    {
        snake.movement[0] = 0;
        snake.movement[1] = -1;
    }
}

void snake_tail()
{
    for (int i = snake.tail_no - 1; i > 0; i--)
    {
        snake.tail_pos[i][0] = snake.tail_pos[i - 1][0];
        snake.tail_pos[i][1] = snake.tail_pos[i - 1][1];
    }

    snake.tail_pos[0][0] = snake.pos[0];
    snake.tail_pos[0][1] = snake.pos[1];
}

bool snake_isTail(int y, int x)
{
    for (int i = 0; i < snake.tail_no; i++)
    {
        if (snake.tail_pos[i][0] == y && snake.tail_pos[i][1] == x)
            return true;
    }
    return false;
}

void snake_postMove()
{
    if (board[snake.pos[0] + snake.movement[0]][snake.pos[1] + snake.movement[1]] == 2)
    {
        finish = true;
    }
    else
    {
        snake.pos[0] += snake.movement[0];
        snake.pos[1] += snake.movement[1];

        for (int i = 0; i < 2; i++)
        {
            if (snake.pos[i] >= i * HEIGHT + (1 - i) * WIDTH)
            {
                snake.pos[i] = 0;
            }else if (snake.pos[i] < 0)
            {
                snake.pos[i] = i * HEIGHT + (1 - i) * WIDTH - 1;
            }
        }
    }
}

void snake_ini()
{
    for (int i = 0; i < MAX_TAIL_NO; i++)
    {
        snake.tail_pos[i][0] = -1;
        snake.tail_pos[i][1] = -1;
    }

    snake.tail_no = 10;
    snake.score = 0;
    snake.highscore = 0;
}

void snake_spawn()
{
    snake.pos[0] = HEIGHT / 2;
    snake.pos[1] = WIDTH / 2;
}

void snake_highscore_ini()
{
    FILE *infile;
    infile = fopen("highscore.txt", "r");

    for (int i = 1000; i >= 1; i /= 10)
    {
        snake.highscore += (fgetc(infile) - '0') * i;
    }

    fclose(infile);
}

void snake_highscore_save()
{
    FILE *outfile;
    outfile = fopen("highscore.txt", "w");

    for (int i = 10000; i > 1; i /= 10)
    {
        fprintf(outfile, "%c", snake.score % i / (i / 10) + '0');
    }

    fclose(outfile);
}

/////////////////////////////////////////////////////////////

void board_sync()
{
    for (int i = 0; i < HEIGHT; i++)
    {
        for (int j = 0; j < WIDTH; j++)
        {
            if (i == snake.pos[0] && j == snake.pos[1])
                board[i][j] = 1;

            else if (snake_isTail(i, j))
                board[i][j] = 2;

            else if (i == apple[0] && j == apple[1])
                board[i][j] = 3;

            else
                board[i][j] = 0;
        }
    }
}

void screen_render(int condition)
{
    int pixel = 0, uipixel = 30;
    unsigned char screen[HEIGHT * (WIDTH * 2 + 1) + uipixel];

    for (int i = 0; i < HEIGHT; i++)
    {
        for (int j = 0; j < WIDTH; j++)
        {
            if (board[i][j] == 1)
            {
                screen[pixel++] = 219;
                screen[pixel++] = 219;
            }
            else if (board[i][j] == 2)
            {
                screen[pixel++] = 178;
                screen[pixel++] = 178;
            }
            else if (board[i][j] == 3)
            {
                screen[pixel++] = '<';
                screen[pixel++] = '>';
            }
            else if (i % 2 == j % 2)
            {
                screen[pixel++] = 177;
                screen[pixel++] = 177;
            }
            else
            {
                screen[pixel++] = 176;
                screen[pixel++] = 176;
            }
        }

        if (i + 1 != HEIGHT)
        {
            if (i == 0)
            {
                screen[pixel++] = ' ';
                screen[pixel++] = 'S';
                screen[pixel++] = 'C';
                screen[pixel++] = 'O';
                screen[pixel++] = 'R';
                screen[pixel++] = 'E';
                screen[pixel++] = ':';
                screen[pixel++] = '\n';
            }
            else if (i == 1)
            {
                screen[pixel++] = ' ';
                screen[pixel++] = snake.score % 10000 / 1000 + '0';
                screen[pixel++] = snake.score % 1000 / 100 + '0';
                screen[pixel++] = snake.score % 100 / 10 + '0';
                screen[pixel++] = snake.score % 10 + '0';
                screen[pixel++] = '\n';
            }
            else if (i == 3)
            {
                screen[pixel++] = ' ';
                screen[pixel++] = 'H';
                screen[pixel++] = 'I';
                screen[pixel++] = 'G';
                screen[pixel++] = 'H';
                screen[pixel++] = 'S';
                screen[pixel++] = 'C';
                screen[pixel++] = 'O';
                screen[pixel++] = 'R';
                screen[pixel++] = 'E';
                screen[pixel++] = ':';
                screen[pixel++] = '\n';
            }
            else if (i == 4)
            {
                screen[pixel++] = ' ';
                screen[pixel++] = snake.highscore % 10000 / 1000 + '0';
                screen[pixel++] = snake.highscore % 1000 / 100 + '0';
                screen[pixel++] = snake.highscore % 100 / 10 + '0';
                screen[pixel++] = snake.highscore % 10 + '0';
                screen[pixel++] = '\n';
            }
            else
            {
                screen[pixel++] = '\n';
            }
        }
        else
        {
            screen[pixel] = '\0';
        }
    }

    strcat((char*)screen, "\n\n");

    if (condition == 1)
    {
        int screen_end_pos[2] = {HEIGHT / 2 - 2, 2 * WIDTH / 2 - 6};
        char screen_end_text[3][13] = {"            ","  YOU DIED  ","            "};

        for (int j = 0; j < 3; j++)
        {
            for (int i = 0; i < 12; i++)
            {
                Sleep(20);
                screen[(2 * WIDTH + 1) * (screen_end_pos[0] + j) + screen_end_pos[1] + i + uipixel] = screen_end_text[j][i];
                SetConsoleCursorPosition(hOut, home_cursor_pos);
                fwrite(screen, 1, strlen((char*)screen), stdout);
            }
        }
    }
    else
    {
        SetConsoleCursorPosition(hOut, home_cursor_pos);
        fwrite(screen, 1, strlen((char*)screen), stdout);
    }
}

//////////////////////////////////////////////////////////////////

void apple_spawn()
{
    int board_unoccupied[HEIGHT * WIDTH][2];
    int unoccupied_count = 0;

    for (int i = 0; i < HEIGHT; i++)
    {
        for (int j = 0; j < WIDTH; j++)
        {
            if (board[i][j] == 0)
            {
                board_unoccupied[unoccupied_count][0] = i;
                board_unoccupied[unoccupied_count++][1] = j;
            }
        }
    }

    int random = rand() % unoccupied_count;

    apple[0] = board_unoccupied[random][0];
    apple[1] = board_unoccupied[random][1];
}


void apple_eat()
{
    if (snake.pos[0] == apple[0] && snake.pos[1] == apple[1])
    {
        apple_spawn();
        snake.tail_no++;
        snake.score++;
    }
}