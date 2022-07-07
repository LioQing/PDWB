#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char** argv) {
    srand(time(NULL));
    
    int upper = 100;
    int lower = 1;
    const int n = rand() % upper + lower;

    int input = 0;
    while (1) {
        fflush(stdin);
        printf("Make a guess (%d - %d): ", lower, upper);
        
        if (scanf("%d", &input) != 1 || input < lower || input > upper) {
            int c;
            while (((c = fgetc(stdin)) != '\n') && (c != EOF));
            continue;
        }

        if (input > n) {
            printf("%d is too large\n", input);
            upper = input - 1;
        } else if (input < n) {
            printf("%d is too small\n", input);
            lower = input + 1;
        } else {
            printf("You are correct\n");
            break;
        }
    }

    return 0;
}
