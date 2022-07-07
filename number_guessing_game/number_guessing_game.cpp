#include <iostream>
#include <limits>
#include <random>

int main(int argc, char** argv) 
{
    int lower = 1;
    int upper = 100;
    std::random_device rnd;
    std::mt19937 gen(rnd());
    std::uniform_int_distribution<> distrib(lower, upper);
    const int n = distrib(gen);

    int input = 0;
    while (true)
    {
        std::cout << "Make a guess (" << lower << " - " << upper << "): ";
        if (!(std::cin >> input) || input < lower || input > upper)
        {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            continue;
        }
        
        if (input > n) 
        {
            std::cout << input << " is too large" << std::endl;
            upper = input - 1;
        } 
        else if (input < n) 
        {
            std::cout << input << " is too small" << std::endl;
            lower = input + 1;
        } 
        else 
        {
            std::cout << "You are correct" << std::endl;
            break;
        }
    }

    return 0;
}