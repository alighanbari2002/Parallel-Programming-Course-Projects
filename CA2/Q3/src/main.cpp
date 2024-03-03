#include <stdio.h>
#include <stdlib.h>
#include <chrono>
#include <sstream>
#include <cassert>
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
using std::stringstream;
using std::locale;

typedef long long ll;

#define IMAGE_01   "../assets/image_01.png"
#define IMAGE_02   "../assets/image_02.png"
#define OUTPUT_DIR "../output/"

const int M128_GRAY_INTERVAL = 16;

ll calculate_absolute_difference_serial(const Mat& img1, const Mat& img2)
{
    Mat out_img_serial(img1.rows, img1.cols, CV_8U);

    // Start the timer
	auto start = high_resolution_clock::now();

    for(int row = 0; row < out_img_serial.rows; ++row)
    {
        for(int col = 0; col < out_img_serial.cols; ++col)
        {
            out_img_serial.at<uchar>(row, col) = abs(
                img1.at<uchar>(row, col) - img2.at<uchar>(row, col)
                );
        }
    }

    // Stop the timer
	auto finish = high_resolution_clock::now();

    imwrite(OUTPUT_DIR "serial output.png", out_img_serial);
    out_img_serial.release();

	ll execution_time = duration_cast<nanoseconds>(finish - start).count();

    // Use a string stream to format the output
	stringstream ss;
	ss.imbue(locale(""));
	ss << execution_time;

    printf("\t- Serial Method: %s\n", ss.str().c_str());

    return execution_time;
}

ll calculate_absolute_difference_parallel(const Mat& img1, const Mat& img2)
{
    Mat out_img_parallel(img1.rows, img1.cols, CV_8U);

    __m128i img1_part, img2_part, diff_img12, diff_img21, diff;

    // Start the timer
	auto start = high_resolution_clock::now();

    for (int row = 0; row < out_img_parallel.rows; ++row)
    {
        for (int col = 0; col < out_img_parallel.cols; col += M128_GRAY_INTERVAL)
        {
            img1_part = _mm_loadu_si128(reinterpret_cast<const __m128i*>(&img1.at<uchar>(row, col)));
            img2_part = _mm_loadu_si128(reinterpret_cast<const __m128i*>(&img2.at<uchar>(row, col)));
            diff_img12 = _mm_subs_epu8(img1_part, img2_part);
            diff_img21 = _mm_subs_epu8(img2_part, img1_part);
            diff = _mm_or_si128(diff_img12, diff_img21); // Safe because one is zero see http://0x80.pl/notesen/2018-03-11-sse-abs-unsigned.html
            _mm_storeu_si128(reinterpret_cast<__m128i*>(&out_img_parallel.at<uchar>(row, col)), diff);
        }
    }

    // Stop the timer
	auto finish = high_resolution_clock::now();

    imwrite(OUTPUT_DIR "parallel output.png", out_img_parallel);
    out_img_parallel.release();

	ll execution_time = duration_cast<nanoseconds>(finish - start).count();

    // Use a string stream to format the output
	stringstream ss;
	ss.imbue(locale(""));
	ss << execution_time;

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
