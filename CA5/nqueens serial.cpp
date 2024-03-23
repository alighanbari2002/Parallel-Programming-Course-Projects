#include <stdio.h>
#include <sstream>
#include <chrono>

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

    for(row = 0; row < CHESS_BOARD_SIZE; ++row)
    {
        try_place_queen(queens, 0, row);
    }
}

int main()
{
    int* queens = new int[CHESS_BOARD_SIZE];

    // Start the timer
	auto start_time = std::chrono::high_resolution_clock::now();

    solve_nqueens(queens);

    // Stop the timer
    auto finish_time = std::chrono::high_resolution_clock::now();

    delete[] queens;

    ll execution_time = std::chrono::duration_cast<std::chrono::nanoseconds>(finish_time - start_time).count();

	// Use a string stream to format the output
	std::stringstream output_formatter;
	output_formatter.imbue(std::locale(""));
	output_formatter << execution_time;

    printf("Number of Solutions: %d\n", solutions_count);
    printf("Run Time (ns): %s\n", output_formatter.str().c_str());

    return 0;
}
