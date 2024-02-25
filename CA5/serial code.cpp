#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <sstream>
#include <chrono>

using std::chrono::high_resolution_clock;
using std::chrono::duration_cast;
using std::chrono::nanoseconds;
using std::stringstream;
using std::locale;

typedef long long ll;

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

    // Start the timer
    auto start = high_resolution_clock::now();

    solve(queens);

    // Stop the timer
    auto finish = high_resolution_clock::now();

    delete queens;

	ll execution_time = duration_cast<nanoseconds>(finish - start).count();

	// Use a string stream to format the output
	stringstream ss;
	ss.imbue(locale(""));
	ss << execution_time;

    printf("Serial Version\n");
    printf("==============\n");
    printf("Number of Solutions: %d\n", solutions_count);
    printf("Run Time (ns): %s\n", ss.str().c_str());

    return 0;
}
