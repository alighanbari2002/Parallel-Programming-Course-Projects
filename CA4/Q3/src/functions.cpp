#include <cstdio>
#include <cstdlib>
#include <vector>
#include <pthread.h>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>

#include "functions.hpp"
#include "utils.hpp"

long long calculate_absolute_difference_serial(const cv::Mat& img1, const cv::Mat& img2)
{
    cv::Mat output_image(img1.size(), img1.type());

	// Start the timer
	auto start_time = get_current_time();

    uchar* out_img_row;

    int row, col;

    for (row = 0; row < output_image.rows; ++row)
    {
        const uchar* img1_row = img1.ptr<uchar>(row);
        const uchar* img2_row = img2.ptr<uchar>(row);
        out_img_row = output_image.ptr<uchar>(row);
        
        for (col = 0; col < output_image.cols; ++col)
        {
            out_img_row[col] = static_cast<uchar>(abs(img1_row[col] - img2_row[col]));
        }
    }

    // Stop the timer
	auto finish_time = get_current_time();

    cv::imwrite(utils::paths::SERIAL_OUTPUT_PATH, output_image);

    release_image(output_image);

	return calculate_duration(start_time, finish_time);
}

void* process_image_rows(void* arg)
{
    thread_data_t* data = (thread_data_t*) arg;

    uchar* out_img_row;

    int row, col;

    for (row = data->start_row; row < data->end_row; ++row)
    {
        const uchar* img1_row = data->img1->ptr<uchar>(row);
        const uchar* img2_row = data->img2->ptr<uchar>(row);
        out_img_row = data->out_img->ptr<uchar>(row);

        for (col = 0; col < data->out_img->cols; ++col)
        {
            out_img_row[col] = static_cast<uchar>(abs(img1_row[col] - img2_row[col]));
        }
    }

    pthread_exit(nullptr);
}

long long calculate_absolute_difference_parallel(const cv::Mat& img1, const cv::Mat& img2, const size_t& num_threads)
{
    cv::Mat output_image(img1.size(), img1.type());

    std::vector<pthread_t> threads(num_threads);
    std::vector<thread_data_t> thread_args(num_threads);

    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    int rows_per_thread = img1.rows / num_threads;

    size_t i;

	// Start the timer
	auto start_time = get_current_time();

    // Assign the arguments for each thread
    for (i = 0; i < num_threads; ++i)
    {
        thread_args[i].img1 = &img1;
        thread_args[i].img2 = &img2;
        thread_args[i].out_img = &output_image;

        thread_args[i].start_row = i * rows_per_thread;
        thread_args[i].end_row = (i == num_threads - 1) ? img1.rows : (i + 1) * rows_per_thread;

        int rc = pthread_create(&threads[i], &attr, process_image_rows, static_cast<void*>(&thread_args[i]));
        if (rc)
        {
            printf("ERROR: return code from pthread_create() is %d\n", rc);
            exit(EXIT_FAILURE);
        }
    }

    // Free the attribute and join the threads
    pthread_attr_destroy(&attr);
    for (i = 0; i < num_threads; ++i)
    {
        int rc = pthread_join(threads[i], NULL);
        if (rc)
        {
            printf("ERROR: return code from pthread_join() is %d\n", rc);
            exit(EXIT_FAILURE);
        }
    }

	// Stop the timer
	auto finish_time = get_current_time();

    cv::imwrite(utils::paths::PARALLEL_OUTPUT_PATH, output_image);

    release_image(output_image);

	return calculate_duration(start_time, finish_time);
}
