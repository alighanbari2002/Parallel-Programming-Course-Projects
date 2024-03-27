#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sstream>
#include <omp.h>

void print_group_info()
{
    printf("Group Members:\n");
    printf("\t- Ali Ghanbari [810199473]\n");
    printf("\t- Behrad Elmi  [810199557]\n");
}

int* create_board(const size_t& board_size)
{
    return new int[board_size];
}

void clean_up_board(int*& board)
{
    delete[] board;
    board = nullptr;
}

double get_current_time()
{
    return omp_get_wtime();
}

long long calculate_duration(const double& start_time, const double& finish_time)
{
    return static_cast<long long>((finish_time - start_time) * 1e9);
}

const char* format_time(const long long& time_ns)
{
    std::stringstream time_formatter;
    time_formatter.imbue(std::locale(""));
    time_formatter << time_ns;
    return time_formatter.str().c_str();
}

void place_queen_or_advance(int*& board, const int current_row, const int current_column, \
                            const size_t board_size, size_t& total_solutions)
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
    
    if (current_row == board_size - 1)
    {
        #pragma omp critical
            total_solutions++;
    }
    else
    {
        for (int next_column = 0; next_column < board_size; ++next_column)
        {
            place_queen_or_advance(board, current_row + 1, next_column, board_size, total_solutions);
        }
    }
}

long long find_all_nqueens_solutions(int*& board, const size_t& board_size, size_t& total_solutions)
{
    int* local_board;
    int starting_column;

    // Start the timer
    double start_time = get_current_time();

    #pragma omp parallel for simd default(shared) private(local_board, starting_column) schedule(static)
        for (starting_column = 0; starting_column < board_size; ++starting_column)
        {
            local_board = create_board(board_size);
            memcpy(local_board, board, board_size * sizeof(int));

            place_queen_or_advance(local_board, 0, starting_column, board_size, total_solutions);

            clean_up_board(local_board);
        }

    // Stop the timer
    double finish_time = get_current_time();

    return calculate_duration(start_time, finish_time);
}

int main()
{
    const size_t BOARD_SIZE = 15;
    size_t total_solutions = 0;
    int* chess_board = create_board(BOARD_SIZE);

    // Set the number of threads
    size_t num_threads = omp_get_max_threads() - 1;
    omp_set_num_threads(num_threads);

    long long elapsed_time = find_all_nqueens_solutions(chess_board, BOARD_SIZE, total_solutions);

    clean_up_board(chess_board);

    print_group_info();
    printf("\nTotal Number of N-Queens Solutions: %zu\n", total_solutions);
    printf("\nRun Time for Parallel Implementation (ns): %s\n", format_time(elapsed_time));

    return EXIT_SUCCESS;
}
