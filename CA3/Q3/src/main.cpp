#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#ifdef 		_WIN32
#include <intrin.h>
#else
#include <x86intrin.h>
#endif

using cv::Mat;
using cv::imwrite;
using cv::imread;
using cv::IMREAD_GRAYSCALE;

#define IMAGE_01   "../assets/image_01.png"
#define IMAGE_02   "../assets/image_02.png"
#define OUTPUT_DIR "../output/"
#define M128_GRAY_INTERVAL 16
#define NUM_THREADS omp_get_max_threads()

// Global variables
Mat img1;
Mat img2;
unsigned int NROWS;
unsigned int NCOLS;

double serial_implementation() {
    Mat out_img_serial(NROWS, NCOLS, CV_8U);
    size_t row, col;

	double start = omp_get_wtime();

    for(row = 0; row < NROWS; ++row) {
        for(col = 0; col < NCOLS; ++col) {
            out_img_serial.at<uchar> (row, col) = abs(
                img1.at<uchar> (row, col) - img2.at<uchar> (row, col)
                );
        }
    }

	double finish = omp_get_wtime();
	double execution_time = (finish - start) * pow(10, 9);

    imwrite(OUTPUT_DIR "serial output.png", out_img_serial);
    out_img_serial.release();

    printf("\t- Serial Method: %.4lf\n", execution_time);

    return execution_time;
}

double parallel_implementation() {
    Mat out_img_parallel(NROWS, NCOLS, CV_8U);
    size_t row, col;

	double start = omp_get_wtime();

    #pragma omp parallel for simd default(shared) private(row, col) schedule(auto) num_threads(NUM_THREADS)
        for(row = 0; row < NROWS; ++row) {
            for(col = 0; col < NCOLS; ++col) {
                out_img_parallel.at<uchar> (row, col) = abs(
                    img1.at<uchar> (row, col) - img2.at<uchar> (row, col)
                    );
            }
        }

	double finish = omp_get_wtime();
	double execution_time = (finish - start) * pow(10, 9);

    imwrite(OUTPUT_DIR "parallel output.png", out_img_parallel);
    out_img_parallel.release();

    printf("\t- Parallel Method: %.4lf\n", execution_time);

    return execution_time;
}

void print_group_info() {
    printf("Group Members:\n");
	printf("\t- Ali Ghanbari [810199473]\n");
	printf("\t- Behrad Elmi  [810199557]\n");
}

int main() {
	print_group_info();

    // Load frames
    img1 = imread(IMAGE_01, IMREAD_GRAYSCALE);
    img2 = imread(IMAGE_02, IMREAD_GRAYSCALE);
    if (img1.size() != img2.size()) {
        printf("Illegal frames!\n");
        exit(EXIT_FAILURE);
    }
    
    NROWS = img1.rows;
    NCOLS = img1.cols;

    printf("\nRun Time (ns):\n");
    double serial_time   = serial_implementation();
	double parallel_time = parallel_implementation();

	printf("\nSpeedup: %.4lf\n", serial_time / parallel_time);

    img1.release();
    img2.release();
    
    return 0;
}
