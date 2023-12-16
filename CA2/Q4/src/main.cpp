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

#define FRONT_IAMGE "../assets/front.png"
#define LOGO_IMAGE  "../assets/logo.png"
#define OUTPUT_DIR  "../output/"
#define M128_GRAY_INTERVAL 16
#define ALPHA 0.25

// Global variables
Mat logo;
Mat front;
unsigned int LOGO_ROW;
unsigned int LOGO_COL;
unsigned int FRONT_ROW;
unsigned int FRONT_COL;

double serial_implementation() {
    Mat out_img_serial(FRONT_ROW, FRONT_COL, CV_8U);

	auto start = high_resolution_clock::now();

    for(size_t row = 0; row < FRONT_ROW; row++) {
        for(size_t col = 0; col < FRONT_COL; col++) {
            if(row <= LOGO_ROW && col <= LOGO_COL) {
                if(front.at<uchar> (row, col) + ALPHA * logo.at<uchar> (row, col) > 255)
                    out_img_serial.at<uchar> (row, col) = 255;
                else
                    out_img_serial.at<uchar> (row, col) = front.at<uchar> (row, col) + ALPHA * logo.at<uchar> (row, col);
            }
            else
                out_img_serial.at<uchar> (row, col) = front.at<uchar> (row, col);
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
    Mat out_img_parallel(FRONT_ROW, FRONT_COL, CV_8U);

    __m128i_u logo_part, front_part;
    __m128i_u* output_part;

	auto start = high_resolution_clock::now();

    for(size_t row = 0; row < FRONT_ROW; row++) {
        for(size_t col = 0; col < FRONT_COL; col += M128_GRAY_INTERVAL) {
            
            logo_part  = _mm_loadu_si128(reinterpret_cast<__m128i_u*>(&logo.at<uchar> (row, col)));
            front_part = _mm_loadu_si128(reinterpret_cast<__m128i_u*>(&front.at<uchar> (row, col)));
            output_part   = reinterpret_cast<__m128i_u*>(&out_img_parallel.at<uchar> (row, col));
                
            if(row <= LOGO_ROW && col <= LOGO_COL - M128_GRAY_INTERVAL) {
                // Divide by 4 
                // 00 --> xxxx xxxx xxxx xx --> xx
                // 00xx xxxx xxxx xxxx --> 00xx xxxx 00xx xxxx

                logo_part = _mm_srli_epi16(logo_part, 2);
                logo_part = _mm_and_si128(logo_part, _mm_set1_epi16(0xFF3F));
                
                // Multiply front by 3 achieving 0.75
                // x <-- xxx xxxx xxxx xxxx <--0
                // xxxx xxxx xxxx xxx0
                // xxxx xxx0 xxxx xxx0
                // pf = _mm_slli_epi16(pf, 1);
                // pf = _mm_and_si128(pf, _mm_set1_epi16(0xFEFF));

                _mm_storeu_si128(output_part, _mm_adds_epu8(logo_part, front_part));
            }
            else
                _mm_storeu_si128(output_part, front_part);
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
    logo  = imread(LOGO_IMAGE, IMREAD_GRAYSCALE);
    front = imread(FRONT_IAMGE, IMREAD_GRAYSCALE);
    if (logo.size().width > front.size().width ||
        logo.size().height > front.size().height) {
        printf("Illegal frames!\n");
        exit(EXIT_FAILURE);
    }

    LOGO_ROW  = logo.rows;
    LOGO_COL  = logo.cols;
    FRONT_ROW = front.rows;
    FRONT_COL = front.cols;

    printf("\nRun Time (ns):\n");
    double serial_time   = serial_implementation();
	double parallel_time = parallel_implementation();

	printf("\nSpeedup: %.4lf\n", serial_time / parallel_time);

    logo.release();
    front.release();

    return 0;
}
