#include <iostream>
#include <limits>
#include <random>

int main(int argc, char** argv) 
{
    auto lower = 1;
    auto upper = 100;
    std::random_device rnd;
    std::mt19937 gen(rnd());
    std::uniform_int_distribution<> distrib(lower, upper);
    int n = distrib(gen);

    for (auto input = 0; input != n;)
    {
        while (std::cout << "Make a guess (" << lower << " - " << upper << "): ", !(std::cin >> input) || input < lower || input > upper)
        {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
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
        }
    }

    return 0;
}