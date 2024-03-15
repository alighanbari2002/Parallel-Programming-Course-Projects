#include <stdio.h>
#include <stdlib.h>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <unistd.h>
#include <pthread.h>
#include <algorithm>
#include <cassert>
#include <sstream>
#include <chrono>
#include <string.h>

using cv::Mat;
using cv::imwrite;
using cv::imread;
using cv::IMREAD_GRAYSCALE;
using std::chrono::high_resolution_clock;
using std::chrono::duration_cast;
using std::chrono::nanoseconds;
using std::stringstream;
using std::locale;
using std::min;

typedef long long ll;

typedef struct {
    const Mat* img1;
    const Mat* img2;
    Mat* out_img;
    int start_row;
    int end_row;
} thread_data_t;

#define IMAGE_01   "../assets/image_01.png"
#define IMAGE_02   "../assets/image_02.png"
#define OUTPUT_DIR "../output/"

ll calculate_absolute_difference_serial(const Mat& img1, const Mat& img2)
{
    Mat out_img_serial(img1.rows, img1.cols, CV_8U);

    // Start the timer
	auto start_time = high_resolution_clock::now();

    for(int row = 0; row < out_img_serial.rows; ++row)
    {
        const uchar* img1_row = img1.ptr<uchar>(row);
        const uchar* img2_row = img2.ptr<uchar>(row);
        uchar* out_img_row = out_img_serial.ptr<uchar>(row);

        for(int col = 0; col < out_img_serial.cols; ++col)
        {
            out_img_row[col] = abs(img1_row[col] - img2_row[col]);
        }
    }

    // Stop the timer
	auto finish_time = high_resolution_clock::now();

    imwrite(OUTPUT_DIR "serial output.png", out_img_serial);
    out_img_serial.release();

	ll execution_time = duration_cast<nanoseconds>(finish_time - start_time).count();

    // Use a string stream to format the output
	stringstream output_formatter;
	output_formatter.imbue(locale(""));
	output_formatter << execution_time;

    printf("\t- Serial Method: %s\n", output_formatter.str().c_str());

    return execution_time;
}

void* process_image_rows(void* arg)
{
    thread_data_t* data = (thread_data_t*) arg;

    for(int row = data->start_row; row < data->end_row; ++row)
    {
        const uchar* img1_row = data->img1->ptr<uchar>(row);
        const uchar* img2_row = data->img2->ptr<uchar>(row);
        uchar* out_img_row = data->out_img->ptr<uchar>(row);

        for(int col = 0; col < data->out_img->cols; ++col)
        {
            out_img_row[col] = abs(img1_row[col] - img2_row[col]);
        }
    }

    pthread_exit(NULL);
}

ll calculate_absolute_difference_parallel(const Mat& img1, const Mat& img2)
{
    Mat out_img_parallel(img1.rows, img1.cols, CV_8U);

    int num_procs = sysconf(_SC_NPROCESSORS_ONLN);
    int num_threads = min(8, num_procs - 1);

    pthread_t threads[num_threads];
    thread_data_t thread_data_array[num_threads];

    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    int rows_per_thread = img1.rows / num_threads;

    int i;

	// Start the timer
	auto start_time = high_resolution_clock::now();

    // Assign the arguments for each thread
    for(i = 0; i < num_threads; ++i)
    {
        thread_data_array[i].img1 = &img1;
        thread_data_array[i].img2 = &img2;
        thread_data_array[i].out_img = &out_img_parallel;
        thread_data_array[i].start_row = i * rows_per_thread;
        thread_data_array[i].end_row = (i == num_threads - 1) ? img1.rows : (i + 1) * rows_per_thread;

        int rc = pthread_create(&threads[i], &attr, process_image_rows, &thread_data_array[i]);
        if (rc)
        {
            printf("ERROR: return code from pthread_create() is %d\n", rc);
            exit(-1);
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
            exit(-1);
        }
    }

	// Stop the timer
	auto finish_time = high_resolution_clock::now();

	ll execution_time = duration_cast<nanoseconds>(finish_time - start_time).count();

	// Use a string stream to format the output
	stringstream output_formatter;
	output_formatter.imbue(locale(""));
	output_formatter << execution_time;

    imwrite(OUTPUT_DIR "parallel output.png", out_img_parallel);
    out_img_parallel.release();

    printf("\t- Parallel Method: %s\n", output_formatter.str().c_str());

    return execution_time;
}

void print_group_info()
{
    printf("Group Members:\n");
	printf("\t- Ali Ghanbari [810199473]\n");
	printf("\t- Behrad Elmi  [810199557]\n");
}

int main()
{
	print_group_info();

    Mat image_01 = imread(IMAGE_01, IMREAD_GRAYSCALE);
    Mat image_02 = imread(IMAGE_02, IMREAD_GRAYSCALE);

    CV_Assert(image_01.size() == image_02.size() && 
              "Illegal frames: image_01 and image_02 have different sizes");

    printf("\nRun Time (ns):\n");
    ll serial_time = calculate_absolute_difference_serial(image_01, image_02);
	ll parallel_time = calculate_absolute_difference_parallel(image_01, image_02);

	printf("\nSpeedup: %.4lf\n", (double) serial_time / (double) parallel_time);

    image_01.release();
    image_02.release();

    return 0;
}
