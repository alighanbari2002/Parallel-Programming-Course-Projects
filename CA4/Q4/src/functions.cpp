#include <cstdio>
#include <cstdlib>
#include <vector>
#include <pthread.h>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>

#include "functions.hpp"
#include "utils.hpp"

long long image_blending_serial(const cv::Mat& front, const cv::Mat& logo)
{
    cv::Mat output_image = front.clone();

	// Start the timer
	auto start_time = get_current_time();

    uchar* out_img_row;

    int row, col, new_pixel;

    for (row = 0; row < logo.rows; ++row)
    {
        const uchar* front_row = front.ptr<uchar>(row);
        const uchar* logo_row = logo.ptr<uchar>(row);
        out_img_row = output_image.ptr<uchar>(row);

        for (col = 0; col < logo.cols; ++col)
        {
            new_pixel = static_cast<int>(front_row[col] + utils::config::ALPHA * logo_row[col]);
            out_img_row[col] = new_pixel > 255 ? 255 : static_cast<uchar>(new_pixel);
        }
    }

    // Stop the timer
	auto finish_time = get_current_time();

    cv::imwrite(utils::paths::SERIAL_OUTPUT_PATH, output_image);

    release_image(output_image);

	return calculate_duration(start_time, finish_time);
}

void* blend_images_with_alpha(void* arg)
{
    thread_data_t* data = (thread_data_t*) arg;

    uchar* out_img_row;
    int row, col, new_pixel;

    for (row = data->start_row; row < data->end_row; ++row)
    {
        const uchar* front_row = data->front->ptr<uchar>(row);
        const uchar* logo_row = data->logo->ptr<uchar>(row);
        out_img_row = data->out_img->ptr<uchar>(row);

        for (col = 0; col < data->logo->cols; ++col)
        {            
            new_pixel = static_cast<int>(front_row[col] + utils::config::ALPHA * logo_row[col]);
            out_img_row[col] = new_pixel > 255 ? 255 : static_cast<uchar>(new_pixel);
        }
    }

    pthread_exit(nullptr);
}

long long image_blending_parallel(const cv::Mat& front, const cv::Mat& logo, const size_t& num_threads)
{
    cv::Mat output_image = front.clone();

    std::vector<pthread_t> threads(num_threads);
    std::vector<thread_data_t> thread_args(num_threads);

    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    int rows_per_thread = logo.rows / num_threads;

    size_t i;

	// Start the timer
	auto start_time = get_current_time();

    // Assign the arguments for each thread
    for (i = 0; i < num_threads; ++i)
    {
        thread_args[i].front = &front;
        thread_args[i].logo = &logo;
        thread_args[i].out_img = &output_image;

        thread_args[i].start_row = i * rows_per_thread;
        thread_args[i].end_row = (i == num_threads - 1) ? logo.rows : (i + 1) * rows_per_thread;

        int rc = pthread_create(&threads[i], &attr, blend_images_with_alpha, static_cast<void*>(&thread_args[i]));
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
