#include <stdio.h>
#include <stdlib.h>
#include <chrono>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#ifdef 		_WIN32
#include <intrin.h>
#else
#include <x86intrin.h>
#endif

using std::chrono::high_resolution_clock;
using std::chrono::duration_cast;
using std::chrono::nanoseconds;
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

double serial_implementation() {
    Mat out_img_serial(NROWS, NCOLS, CV_8U);

	auto start = high_resolution_clock::now();

    for(size_t row = 0; row < NROWS; row++) {
        for(size_t col = 0; col < NCOLS; col++) {
            out_img_serial.at<uchar> (row, col) = abs(
                img1.at<uchar> (row, col) - img2.at<uchar> (row, col)
                );
        }
    }

	auto finish = high_resolution_clock::now();
	double execution_time = duration_cast<nanoseconds>(finish - start).count();

    imwrite(OUTPUT_DIR "serial output.png", out_img_serial);
    out_img_serial.release();

    printf("\t- Serial Method: %.4lf\n", execution_time);

    return execution_time;
}

double parallel_implementation() {
    Mat out_img_parallel(NROWS, NCOLS, CV_8U);

    __m128i img1_part, img2_part, diff_img12, diff_img21, diff;

	auto start = high_resolution_clock::now();
    
    for (size_t row = 0; row < NROWS; row++) {
        for (size_t col = 0; col < NCOLS; col += M128_GRAY_INTERVAL) {
            img1_part = _mm_loadu_si128(reinterpret_cast<__m128i*>(&img1.at<uchar> (row, col)));
            img2_part = _mm_loadu_si128(reinterpret_cast<__m128i*>(&img2.at<uchar> (row, col)));
            diff_img12 = _mm_subs_epu8(img1_part, img2_part);
            diff_img21 = _mm_subs_epu8(img2_part, img1_part);
            diff = _mm_or_si128(diff_img12, diff_img21); // Safe because one is zero see http://0x80.pl/notesen/2018-03-11-sse-abs-unsigned.html
            _mm_storeu_si128(reinterpret_cast<__m128i*>(&out_img_parallel.at<uchar> (row, col)), diff);
        }
    }

	auto finish = high_resolution_clock::now();
	double execution_time = duration_cast<nanoseconds>(finish - start).count();

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
