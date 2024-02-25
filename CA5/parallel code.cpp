#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <math.h>
#include <chrono>

using std::chrono::high_resolution_clock;
using std::chrono::duration_cast;
using std::chrono::nanoseconds;

#define N 15
#define NUM_THREADS omp_get_max_threads() - 1

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

    #pragma omp critical
        queens[row] = column;
    
    if(row == N - 1)
    {
		#pragma omp critical
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

	#pragma omp parallel for default(shared) private(i) schedule(auto) num_threads(NUM_THREADS)
		for(i = 0; i < N; ++i)
		{
			put(queens, 0, i);
		}
}

int main()
{
    int* queens = new int[N];

    // Start the timer
    auto start = high_resolution_clock::now();

    solve(queens);

    // Stop the timer
    auto finish = high_resolution_clock::now();

    delete queens;

	double execution_time = duration_cast<nanoseconds>(finish - start).count();

    printf("Parallel Version\n");
    printf("================\n");
    printf("Number of solutions: %d\n", solutions_count);
    printf("Time taken: %lf (ns)\n", execution_time);

    return 0;
}
