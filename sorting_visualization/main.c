#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <time.h>

int compareArray();

void sorting();
void merge();
void insertion();
void bubble();

void shuffle();
void drawGraph();
void selection();

int framerate = 50;
int arr_size = 35;
int selected = 0;
long arr[35];
unsigned char draw[2 * 36 * 35];

int main()
{
    srand(time(NULL));

    for (int i = 0; i < arr_size; i++)
    {
        arr[i] = i + 1;
    }

    shuffle(arr, arr_size);

    selection();

    drawGraph();

    sorting();

    drawGraph();

    return 0;
}

void shuffle(int *array, size_t n)
{
    if (n > 1)
    {
        size_t i;
        for (i = 0; i < n - 1; i++)
        {
          size_t j = i + rand() / (RAND_MAX / (n - i) + 1);
          int t = array[j];
          array[j] = array[i];
          array[i] = t;
        }
    }
}

void drawGraph(int highlightPos)
{
    int pos = 0;

    Sleep(framerate);

    for (int i = 0; i < arr_size; i++)
    {
        for (int j = 0; j < arr_size; j++)
        {
            if (arr_size - i <= arr[j] && j == highlightPos)
                draw[pos++] = 178;
            else if (arr_size - i <= arr[j])
                draw[pos++] = 177;
            else
                draw[pos++] = ' ';

            draw[pos++] = ' ';
        }
        draw[pos++] = '\n';
    }

    printf("%s\n", draw);
}

void selection()
{
    selected = 0; //1 - Insertion 2 - Bubble

    printf("Sorting algorithms:\n\n1 - Insertion Sort\n2 - Bubble Sort\n3 - Merge Sort\n\n");

    while (selected < 1 || selected > 3)
    {
        printf("Please enter a sorting algorithm: ");
        scanf("%d", &selected);
    }
    printf("\n");
}

void sorting()
{
    if (selected == 1)
    {
        insertion();
    }
    else if (selected == 2)
    {
        bubble();
    }
    else if (selected == 3)
    {
        merge(1, arr_size);
    }
}

void insertion()
{
    for (int i = 1; i < arr_size; i++)
    {
        int temp_pos = i;

        while (arr[i] < arr[i - 1])
        {
            int temp = arr[i];
            arr[i] = arr[i - 1];
            arr[i - 1] = temp;
            i--;

            drawGraph(i);
        }
    }
}

void bubble()
{
    for (int j = arr_size - 1; j >= 0; j--)
    {
        for (int i = 0; i < j; i++)
        {
            int temp;

            if (arr[i] > arr[i + 1])
            {
                temp = arr[i];
                arr[i] = arr[i + 1 ];
                arr[i + 1] = temp;
            }

            drawGraph(i + 1);
        }

        if (arr[j] - 1 == arr[j - 1])
            j--;
    }
}

void merge(int l, int r)
{
    int step[4], m = (l + r) / 2;

    if (l == r) return;

    merge(l, m);
    merge(m + 1, r);

    step[0] = l - 1;
    step[1] = m - 1;
    step[2] = m;
    step[3] = r - 1;

    int len = step[3] - step[0] + 1;
    long int temp_arr[len];

    for (int i = 0, n = step[0], m = step[2]; i < len; i++)
    {
        if (m == step[3] + 1 || n != step[1] + 1 && arr[n] < arr[m])
        {
            drawGraph(n);
            temp_arr[i] = arr[n++];
        } else
        {
            drawGraph(m);
            temp_arr[i] = arr[m++];
        }
    }

    for (int i = 0, j = step[0]; i < len; i++, j++)
    {
        arr[j] = temp_arr[i];
        drawGraph(j);
    }
}

int compareArray (int a[], int b[], int size)
{
	int i;

	for (i = 0; i < size; i++)
    {
		if(a[i]!=b[i])
			return 1;
	}

	return 0;
}