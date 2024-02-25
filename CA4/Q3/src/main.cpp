#include <stdio.h>
#include <stdlib.h>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <unistd.h>
#include <pthread.h>
#include <sstream>
#include <chrono>

using cv::Mat;
using cv::imwrite;
using cv::imread;
using cv::IMREAD_GRAYSCALE;
using std::chrono::high_resolution_clock;
using std::chrono::duration_cast;
using std::chrono::nanoseconds;
using std::stringstream;
using std::locale;

typedef long long ll;

typedef struct {
    size_t row;
    Mat img1, img2;
    Mat out_img;
} ThreadData;

#define IMAGE_01   "../assets/image_01.png"
#define IMAGE_02   "../assets/image_02.png"
#define OUTPUT_DIR "../output/"
#define NUM_THREADS static_cast<size_t>(sysconf(_SC_NPROCESSORS_ONLN) - 1)

// Global variables
Mat img1;
Mat img2;
unsigned int NROWS;
unsigned int NCOLS;

ll serial_implementation()
{
    Mat out_img_serial(NROWS, NCOLS, CV_8U);
    size_t row, col;

	// Start the timer
	auto start = high_resolution_clock::now();

    for(row = 0; row < NROWS; ++row)
    {
        for(col = 0; col < NCOLS; ++col)
        {
            out_img_serial.at<uchar> (row, col) = abs(
                img1.at<uchar> (row, col) - img2.at<uchar> (row, col)
                );
        }
    }

	// Stop the timer
	auto finish = high_resolution_clock::now();

	ll execution_time = duration_cast<nanoseconds>(finish - start).count();

	// Use a string stream to format the output
	stringstream ss;
	ss.imbue(locale(""));
	ss << execution_time;

    imwrite(OUTPUT_DIR "serial output.png", out_img_serial);
    out_img_serial.release();

    printf("\t- Serial Method: %s\n", ss.str().c_str());

    return execution_time;
}

void* process_image(void* arg)
{
    ThreadData* data = (ThreadData*)arg;
    size_t start_row = data->row * (NROWS / NUM_THREADS);
    size_t end_row = (data->row + 1) * (NROWS / NUM_THREADS);

    for (size_t row = start_row; row < end_row; ++row)
    {
        for (size_t col = 0; col < NCOLS; ++col)
        {
            uchar diff = abs(data->img1.at<uchar>(row, col) - data->img2.at<uchar>(row, col));
            data->out_img.at<uchar>(row, col) = diff;
        }
    }

    pthread_exit(NULL);
}

ll parallel_implementation()
{
    Mat out_img_parallel(NROWS, NCOLS, CV_8U);
    size_t i;

    pthread_t threads[NUM_THREADS];
    ThreadData thread_data_array[NUM_THREADS];

    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

	// Start the timer
	auto start = high_resolution_clock::now();

    // Assign the arguments for each thread
    for (i = 0; i < NUM_THREADS; ++i)
    {
        thread_data_array[i].row = i;
        thread_data_array[i].img1 = img1;
        thread_data_array[i].img2 = img2;
        thread_data_array[i].out_img = out_img_parallel;

        int rc = pthread_create(&threads[i], &attr, process_image, &thread_data_array[i]);
        if (rc)
        {
            printf("ERROR: return code from pthread_create() is %d\n", rc);
            exit(-1);
        }
    }

    // Free the attribute and join the threads
    pthread_attr_destroy(&attr);
    for (i = 0; i < NUM_THREADS; ++i)
    {
        int rc = pthread_join(threads[i], NULL);
        if (rc)
        {
            printf("ERROR: return code from pthread_join() is %d\n", rc);
            exit(-1);
        }
    }

	// Stop the timer
	auto finish = high_resolution_clock::now();

	ll execution_time = duration_cast<nanoseconds>(finish - start).count();

	// Use a string stream to format the output
	stringstream ss;
	ss.imbue(locale(""));
	ss << execution_time;

    imwrite(OUTPUT_DIR "parallel output.png", out_img_parallel);
    out_img_parallel.release();

    printf("\t- Parallel Method: %s\n", ss.str().c_str());
    
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

    // Load frames
    img1 = imread(IMAGE_01, IMREAD_GRAYSCALE);
    img2 = imread(IMAGE_02, IMREAD_GRAYSCALE);
    if (img1.size() != img2.size())
    {
        printf("Illegal frames!\n");
        exit(EXIT_FAILURE);
    }
    
    NROWS = img1.rows;
    NCOLS = img1.cols;

    printf("\nRun Time (ns):\n");
    ll serial_time = serial_implementation();
	ll parallel_time = parallel_implementation();

	printf("\nSpeedup: %.4lf\n", (double)serial_time / (double)parallel_time);

    img1.release();
    img2.release();
    
    return 0;
}
