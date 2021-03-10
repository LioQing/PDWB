#include <iostream>
#include <fstream>
#include <string>
#include <chrono>

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
	#include <windows.h>
#else
	#include <unistd.h>
    #if __APPLE__
        #include <thread>
    #endif
#endif

// clear
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
    #define ClearScreen() SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), { 0, 0 })
#else
	#define ClearScreen() printf("\033[%d;%dH", 1, 1)
#endif

// sleep
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
	#define sleep(ms) Sleep(ms)
#else
	#define sleep(ms) usleep(ms * 1000)
#endif

// play music
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
    #define play_sound(filename) PlaySound(TEXT(filename), NULL, SND_FILENAME | SND_ASYNC)
#endif

// time step
#define TIMESTEP 1000.0 / 15.0

void afplay()
{
    system("afplay bad_apple.wav");
}

int main()
{
    std::ifstream video_file("bad_apple.txt");

    std::string str;
    std::chrono::steady_clock::time_point begin, end;
    uint32_t et = 0;
    uint32_t td = 0;

    #if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
    play_sound("bad_apple.wav");
    sleep(120);
    #elif __APPLE__
    std::thread t(afplay);
    sleep(200);
    #endif
    begin = std::chrono::steady_clock::now();
    for (uint32_t n = 1;; ++n)
    {
        std::getline(video_file, str, 'S');
        ClearScreen();
        std::cout << str << std::endl;

        td = n * TIMESTEP;
            
        end = std::chrono::steady_clock::now();
        et = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();

        if (et < td)
            sleep((uint32_t)(td - et));
    }

    video_file.close();
    t.join();

    std::cin.get();

    return 0;
}