#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef struct bound bound;

struct bound {
    short lower;
    short upper;
};


int main(int argc, char** argv) {
    srand(time(NULL));
    bound range = {1, 100};
    int n = rand() % range.upper + 1;
    int input = 0;

    do {
        fflush(stdin);
        printf("Make a guess (%d - %d): ", range.upper, range.lower);
        if (!scanf("%d", &input)) continue;
        if (input > n) {
            printf("%d is too large\n", input);
            range.upper = input - 1;
        } else if (input < n) {
            printf("%d is too small\n", input);
            range.lower = input + 1;
        }
    } while (input != n);

    printf("You are correct\n");
    return 0;
}
