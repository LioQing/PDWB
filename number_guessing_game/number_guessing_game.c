#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int lower = 1, upper = 100;
int n;

int main(int argc, char** argv) {
    srand(time(NULL));

    n = rand() % 100 + 1;

    for (int input = 0; input != n;) {
        while (printf("Make a guess (%d - %d): ", lower, upper), scanf("%d", &input) != 1 || input < lower || input > upper) {
            int c;
            while (c = fgetc(stdin) != '\n' && c != EOF);
        }

        if (input > n) {
            printf("%d is too large\n", input);
            upper = input - 1;
        } else if (input < n) {
            printf("%d is too small\n", input);
            lower = input + 1;
        } else {
            printf("You are correct\n");
        }
    }

    return 0;
}