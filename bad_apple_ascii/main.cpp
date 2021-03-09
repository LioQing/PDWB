#include <iostream>
#include <fstream>
#include <string>
#include <chrono>

#ifdef _WIN32
	#include <windows.h>
#else
	#include <unistd.h>
#endif

// clear
#ifdef _WIN32
    #define ClearScreen() SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), { 0, 0 })
#else
	#define ClearScreen() printf("\033[%d;%dH", 1, 1)
#endif

// sleep
#ifdef _WIN32
	#define sleep(ms) Sleep(ms)
#else
	#define sleep(ms) usleep(ms * 1000)
#endif

// play music
#ifdef _WIN32
    #define play_sound(filename) PlaySound(TEXT(filename), NULL, SND_FILENAME | SND_ASYNC)
#endif

// time step
#ifdef _WIN32
	#define TimeStep 66.f
#else
	#define TimeStep 66.64f
#endif

int main()
{
    std::ifstream video_file("bad_apple.txt");

    std::string str;
    std::chrono::steady_clock::time_point begin, end;
    float dt, target_dt = TimeStep;
    #ifdef _WIN32
    play_sound("bad_apple.wav");
    #endif
    for (;;)
    {
        begin = std::chrono::steady_clock::now();

        std::getline(video_file, str, 'S');
        ClearScreen();
        std::cout << str << std::endl;
            
        end = std::chrono::steady_clock::now();

        dt = std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() / 1000.f;
        if (dt < target_dt)
        {
            sleep((uint32_t)(target_dt - dt));
            target_dt = TimeStep;
        }
        else if (dt > target_dt)
            target_dt = TimeStep - dt + target_dt;
    }

    video_file.close();

    std::cin.get();

    return 0;
}