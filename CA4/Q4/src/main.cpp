#include <stdio.h>
#include <stdlib.h>
#include <type_traits>
#include <unistd.h>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
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
    const Mat* front;
    const Mat* logo;
    Mat* out_img;
    double alpha;
    int start_row;
    int end_row;
} thread_data_t;

#define FRONT_IAMGE "../assets/front.png"
#define LOGO_IMAGE  "../assets/logo.png"
#define OUTPUT_DIR  "../output/"

const double ALPHA = 0.25;

ll image_blending_serial(const Mat& front, const Mat& logo, double alpha)
{
    Mat out_img_serial = front.clone();

    // Start the timer
	auto start_time = high_resolution_clock::now();

    for(int row = 0; row < out_img_serial.rows; ++row)
    {
        const uchar* front_row = front.ptr<uchar>(row);
        const uchar* logo_row = logo.ptr<uchar>(row);
        uchar* out_img_row = out_img_serial.ptr<uchar>(row);

        for(int col = 0; col < out_img_serial.cols; ++col)
        {
            if(row < logo.rows && col < logo.cols)
            {
                int new_pixel = front_row[col] + alpha * logo_row[col];
                out_img_row[col] = new_pixel > 255 ? 255 : static_cast<uchar>(new_pixel);
            }
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

void* blend_images_with_alpha(void* arg)
{
    thread_data_t* data = (thread_data_t*) arg;

    for(int row = data->start_row; row < data->end_row; ++row)
    {
        const uchar* front_row = data->front->ptr<uchar>(row);
        const uchar* logo_row = data->logo->ptr<uchar>(row);
        uchar* out_img_row = data->out_img->ptr<uchar>(row);

        for(int col = 0; col < data->out_img->cols; ++col)
        {            
            if(row < data->logo->rows && col < data->logo->cols)
            {
                int new_pixel = front_row[col] + data->alpha * logo_row[col];
                out_img_row[col] = new_pixel > 255 ? 255 : static_cast<uchar>(new_pixel);
            }
        }
    }

    pthread_exit(NULL);
}

ll image_blending_parallel(const Mat& front, const Mat& logo, double alpha)
{
    Mat out_img_parallel = front.clone();

    int num_procs = sysconf(_SC_NPROCESSORS_ONLN);
    int num_threads = min(12, num_procs - 1);

    pthread_t threads[num_threads];
    thread_data_t thread_data_array[num_threads];

    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    int rows_per_thread = out_img_parallel.rows / num_threads;
    int remainder = out_img_parallel.rows % num_threads;

    int i;

	// Start the timer
	auto start_time = high_resolution_clock::now();

    // Assign the arguments for each thread
    for(i = 0; i < num_threads; ++i)
    {
        thread_data_array[i].front = &front;
        thread_data_array[i].logo = &logo;
        thread_data_array[i].out_img = &out_img_parallel;
        thread_data_array[i].alpha = alpha;
        thread_data_array[i].start_row = i * rows_per_thread;
        thread_data_array[i].end_row = (i + 1) * rows_per_thread;

        // Adjust the row range for the last thread if there is a remainder
        if(i == num_threads - 1 && remainder > 0)
        {
            thread_data_array[i].end_row += remainder;
        }

        int rc = pthread_create(&threads[i], &attr, blend_images_with_alpha, &thread_data_array[i]);
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

    Mat front_image = imread(FRONT_IAMGE, IMREAD_GRAYSCALE);
    Mat logo_image = imread(LOGO_IMAGE, IMREAD_GRAYSCALE);

    CV_Assert(logo_image.size().width <= front_image.size().width && 
              logo_image.size().height <= front_image.size().height && 
              "Illegal frames: logo_image is larger than front_image");

    printf("\nRun Time (ns):\n");
    ll serial_time = image_blending_serial(front_image, logo_image, ALPHA);
	ll parallel_time = image_blending_parallel(front_image, logo_image, ALPHA);

	printf("\nSpeedup: %.4lf\n", (double) serial_time / (double) parallel_time);

    front_image.release();
    logo_image.release();

    return 0;
}
