#include <cstdio>
#include <cstdlib>
#include <sstream>
#include <string>
#include <chrono>

namespace config {

    constexpr size_t BOARD_SIZE = 15;

} // namespace config

size_t total_solutions;

void print_group_info()
{
    printf("Group Members:\n");
    printf("\t- Ali Ghanbari [810199473]\n");
    printf("\t- Behrad Elmi  [810199557]\n");
}

int* create_board()
{
    return new int[config::BOARD_SIZE];
}

void clean_up_board(int*& board)
{
    delete[] board;
    board = nullptr;
}

std::chrono::high_resolution_clock::time_point get_current_time()
{
	return std::chrono::high_resolution_clock::now();
}

template <typename T>
long long calculate_duration(const T& start_time, const T& finish_time)
{
    return std::chrono::duration_cast<std::chrono::nanoseconds>(finish_time - start_time).count();
}

std::string format_time(const long long& time_ns)
{
    std::stringstream time_formatter;
    time_formatter.imbue(std::locale(""));
    time_formatter << time_ns;
    return time_formatter.str();
}

void place_queen_or_advance(int*& board, const int current_row, const int current_column)
{
    for (int previous_row = 0; previous_row < current_row; ++previous_row)
    {
        int column_difference = abs(board[previous_row] - current_column);
        bool same_column = board[previous_row] == current_column;
        bool same_diagonal = column_difference == (current_row - previous_row);

        if (same_column || same_diagonal)
        {
            return;
        }
    }

    board[current_row] = current_column;

    if (current_row == config::BOARD_SIZE - 1)
    {
        total_solutions++;
    }
    else
    {
        for (int next_column = 0; next_column < config::BOARD_SIZE; ++next_column)
        {
            place_queen_or_advance(board, current_row + 1, next_column);
        }
    }
}

long long find_all_nqueens_solutions(int*& board)
{
    int starting_column;

	// Start the timer
	auto start_time = get_current_time();

    for (starting_column = 0; starting_column < config::BOARD_SIZE; ++starting_column)
    {
        place_queen_or_advance(board, 0, starting_column);
    }

	// Stop the timer
	auto finish_time = get_current_time();

    return calculate_duration(start_time, finish_time);
}

int main()
{
    int* chess_board = create_board();

    long long elapsed_time = find_all_nqueens_solutions(chess_board);

    clean_up_board(chess_board);

    print_group_info();
    printf("\nTotal Number of N-Queens Solutions: %zu\n", total_solutions);
    printf("\nRun Time for Serial Implementation (ns): %s\n", format_time(elapsed_time).c_str());

    return EXIT_SUCCESS;
}
