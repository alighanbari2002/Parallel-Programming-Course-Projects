#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <math.h>
#include <time.h>

#define N 15

int solutions_count;

int put(int* queens, int row, int column)
{
    int i;

    for(i = 0; i < row; ++i)
    {
        if (queens[i] == column || abs(queens[i] - column) == (row - i))
        {
            return -1;
        }
    }

    queens[row] = column; 
    
    if(row == N - 1)
    {
        solutions_count++;
    }
    else
    {
        for(i = 0; i < N; ++i) // increment row
        {
            put(queens, row + 1, i);
        }
    }

    return 0;
}

void solve(int* queens)
{
    int i;
    
    for(i = 0; i < N; ++i)
    {
        put(queens, 0, i);
    }
}

int main()
{
    int queens[N];
    time_t start = 0, finish = 0;    

    start = time(NULL);
    solve(queens);
    finish = time(NULL);

    printf("Number of solutions: %d", solutions_count);
    printf("Time taken: %d", difftime(finish, start));

    return 0;
}
