#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <math.h>
#include <chrono>

using std::chrono::high_resolution_clock;
using std::chrono::duration_cast;
using std::chrono::nanoseconds;

#define N 15

int solutions_count;

int put(int*& queens, int row, int column)
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

void solve(int*& queens)
{
    int i;

    for(i = 0; i < N; ++i)
    {
        put(queens, 0, i);
    }
}

int main()
{
    int* queens = new int[N];

    auto start = high_resolution_clock::now();

    solve(queens);

    auto finish = high_resolution_clock::now();
	double execution_time = duration_cast<nanoseconds>(finish - start).count();

    printf("Number of solutions: %d\n", solutions_count);
    printf("Time taken: %lf (ns)\n", execution_time);

    return 0;
}
