#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <omp.h>
#include <sstream>
#include <string.h>

using ll = long long;

#define CHESS_BOARD_SIZE 15

int solutions_count;

int try_place_queen(int*& queens, int row, int column)
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

    if(row == CHESS_BOARD_SIZE - 1)
    {
		#pragma omp critical
        	solutions_count++;
    }
    else
    {
		for(i = 0; i < CHESS_BOARD_SIZE; ++i) // increment row
		{
			try_place_queen(queens, row + 1, i);
		}
    }

    return 0;
}

void solve_nqueens(int*& queens)
{
	int row;

	int num_threads = omp_get_max_threads();
	omp_set_num_threads(num_threads - 1);

	#pragma omp parallel for default(shared) private(row) schedule(auto)
		for(row = 0; row < CHESS_BOARD_SIZE; ++row)
		{
			try_place_queen(queens, 0, row);
		}
}

int main()
{
    int* queens = new int[CHESS_BOARD_SIZE];

    // Start the timer
    auto start_time = omp_get_wtime();

    solve_nqueens(queens);

    // Stop the timer
	double finish_time = omp_get_wtime();

    delete[] queens;

	ll execution_time = (finish_time - start_time) * pow(10, 9);

	// Use a string stream to format the output
	std::stringstream output_formatter;
	output_formatter.imbue(std::locale(""));
	output_formatter << execution_time;

    printf("Number of Solutions: %d\n", solutions_count);
    printf("Run Time (ns): %s\n", output_formatter.str().c_str());

    return 0;
}
