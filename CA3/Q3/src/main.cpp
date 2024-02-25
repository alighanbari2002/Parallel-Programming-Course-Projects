#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>

using cv::Mat;
using cv::imwrite;
using cv::imread;
using cv::IMREAD_GRAYSCALE;

#define IMAGE_01   "../assets/image_01.png"
#define IMAGE_02   "../assets/image_02.png"
#define OUTPUT_DIR "../output/"
#define M128_GRAY_INTERVAL 16

// Global variables
Mat img1;
Mat img2;
unsigned int NROWS;
unsigned int NCOLS;

double serial_implementation()
{
    Mat out_img_serial(NROWS, NCOLS, CV_8U);
    size_t row, col;

	// Start the timer
	double start = omp_get_wtime();

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
	double finish = omp_get_wtime();

    imwrite(OUTPUT_DIR "serial output.png", out_img_serial);
    out_img_serial.release();

	double execution_time = (finish - start) * pow(10, 9);

    printf("\t- Serial Method: %.4lf\n", execution_time);

    return execution_time;
}

double parallel_implementation()
{
    Mat out_img_parallel(NROWS, NCOLS, CV_8U);
    size_t row, col;

	// Start the timer
	double start = omp_get_wtime();

    #pragma omp parallel for simd default(shared) private(row, col) schedule(auto)
        for(row = 0; row < NROWS; ++row)
        {
            for(col = 0; col < NCOLS; ++col)
            {
                out_img_parallel.at<uchar> (row, col) = abs(
                    img1.at<uchar> (row, col) - img2.at<uchar> (row, col)
                    );
            }
        }

	// Stop the timer
	double finish = omp_get_wtime();

    imwrite(OUTPUT_DIR "parallel output.png", out_img_parallel);
    out_img_parallel.release();

	double execution_time = (finish - start) * pow(10, 9);

    printf("\t- Parallel Method: %.4lf\n", execution_time);

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

	int num_threads = omp_get_max_threads() - 1;
	omp_set_num_threads(num_threads);

    printf("\nRun Time (ns):\n");
    double serial_time   = serial_implementation();
	double parallel_time = parallel_implementation();

	printf("\nSpeedup: %.4lf\n", serial_time / parallel_time);

    img1.release();
    img2.release();
    
    return 0;
}
