#include <iostream>
#include <sstream>
#include <cstdio>
#include <cstdlib>
#include <chrono>
#include <unistd.h>
#include <pthread.h>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>

typedef struct {
    const cv::Mat* img1;
    const cv::Mat* img2;
    cv::Mat* out_img;
    int start_row;
    int end_row;
} thread_data_t;

#define IMAGE_01_PATH "../assets/image_01.png"
#define IMAGE_02_PATH "../assets/image_02.png"
#define OUTPUT_DIR    "../output/"

void print_group_info()
{
    printf("Group Members:\n");
	printf("\t- Ali Ghanbari [810199473]\n");
	printf("\t- Behrad Elmi  [810199557]\n");
}

cv::Mat load_image(const char* const &path)
{
    cv::Mat img = cv::imread(path, cv::IMREAD_GRAYSCALE);
    CV_Assert(!img.empty());
    return img;
}

void release_image(cv::Mat& img)
{
    img.release();
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

const char* format_time(const long long& time_ns)
{
    std::stringstream time_formatter;
    time_formatter.imbue(std::locale(""));
    time_formatter << time_ns;
    return time_formatter.str().c_str();
}

long long calculate_absolute_difference_serial(const cv::Mat& img1, const cv::Mat& img2)
{
    cv::Mat output_image(img1.size(), img1.type());

	// Start the timer
	auto start_time = get_current_time();

    const uchar* img1_row;
    const uchar* img2_row;
    uchar* out_img_row;

    int row, col;

    for (row = 0; row < output_image.rows; ++row)
    {
        img1_row = img1.ptr<uchar>(row);
        img2_row = img2.ptr<uchar>(row);
        out_img_row = output_image.ptr<uchar>(row);
        
        for (col = 0; col < output_image.cols; ++col)
        {
            out_img_row[col] = abs(img1_row[col] - img2_row[col]);
        }
    }

    // Stop the timer
	auto finish_time = get_current_time();

    cv::imwrite(OUTPUT_DIR "serial output.png", output_image);

    release_image(output_image);

	return calculate_duration(start_time, finish_time);
}

void* process_image_rows(void* arg)
{
    thread_data_t* data = (thread_data_t*) arg;

    const uchar* img1_row;
    const uchar* img2_row;
    uchar* out_img_row;

    int row, col;

    for (row = data->start_row; row < data->end_row; ++row)
    {
        img1_row = data->img1->ptr<uchar>(row);
        img2_row = data->img2->ptr<uchar>(row);
        out_img_row = data->out_img->ptr<uchar>(row);

        for (col = 0; col < data->out_img->cols; ++col)
        {
            out_img_row[col] = abs(img1_row[col] - img2_row[col]);
        }
    }

    pthread_exit(nullptr);
}

long long calculate_absolute_difference_parallel(const cv::Mat& img1, const cv::Mat& img2, const size_t& num_threads)
{
    cv::Mat output_image(img1.size(), img1.type());

    pthread_t threads[num_threads];
    thread_data_t thread_args[num_threads];

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

    cv::imwrite(OUTPUT_DIR "parallel output.png", output_image);

    release_image(output_image);

	return calculate_duration(start_time, finish_time);
}

int main()
{
	print_group_info();

    cv::Mat image_01 = load_image(IMAGE_01_PATH);
    cv::Mat image_02 = load_image(IMAGE_02_PATH);

    CV_Assert(image_01.size() == image_02.size() && image_01.type() == image_02.type());

	// Set the number of threads
    size_t num_threads = 4;

    long long elapsed_time_serial = calculate_absolute_difference_serial(image_01, image_02);
	long long elapsed_time_parallel = calculate_absolute_difference_parallel(image_01, image_02, num_threads);

    release_image(image_01);
    release_image(image_02);

    printf("\nRun Time (ns):\n");
    printf("\t- Serial Method: %s\n", format_time(elapsed_time_serial));
    printf("\t- Parallel Method: %s\n", format_time(elapsed_time_parallel));
    printf("\nSpeedup: %.4lf\n", static_cast<double>(elapsed_time_serial) / elapsed_time_parallel);
    
    pthread_exit(nullptr);
}
