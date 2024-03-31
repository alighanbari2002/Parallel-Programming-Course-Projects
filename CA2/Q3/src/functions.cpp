#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#ifdef _WIN32
#include <intrin.h>
#else
#include <x86intrin.h>
#endif

#include "functions.hpp"
#include "utils.hpp"

long long calculate_absolute_difference_serial(const cv::Mat& img1, const cv::Mat& img2)
{
    cv::Mat output_image(img1.size(), img1.type());

    uchar* out_img_row;

    int row, col;

	// Start the timer
	auto start_time = get_current_time();

    for (row = 0; row < output_image.rows; ++row)
    {
        const uchar* img1_row = img1.ptr<uchar>(row);
        const uchar* img2_row = img2.ptr<uchar>(row);
        out_img_row = output_image.ptr<uchar>(row);
        
        for (col = 0; col < output_image.cols; ++col)
        {
            out_img_row[col] = static_cast<uchar>(abs(img1_row[col] - img2_row[col]));
        }
    }

    // Stop the timer
	auto finish_time = get_current_time();

    cv::imwrite(utils::paths::SERIAL_OUTPUT_PATH, output_image);

    release_image(output_image);

	return calculate_duration(start_time, finish_time);
}

long long calculate_absolute_difference_parallel(const cv::Mat& img1, const cv::Mat& img2)
{
    cv::Mat output_image(img1.size(), img1.type());

    uchar* out_img_row;

    __m128i img1_part, img2_part, diff_img12, diff_img21, diff;

    int row, col;

	// Start the timer
	auto start_time = get_current_time();

    for (row = 0; row < output_image.rows; ++row)
    {
        const uchar* img1_row = img1.ptr<uchar>(row);
        const uchar* img2_row = img2.ptr<uchar>(row);
        out_img_row = output_image.ptr<uchar>(row);

        for (col = 0; col < output_image.cols; col += utils::simd::GRAY_INTERVAL)
        {
            img1_part = _mm_loadu_si128(reinterpret_cast<const __m128i*>(img1_row + col));
            img2_part = _mm_loadu_si128(reinterpret_cast<const __m128i*>(img2_row + col));
            diff_img12 = _mm_subs_epu8(img1_part, img2_part);
            diff_img21 = _mm_subs_epu8(img2_part, img1_part);
            diff = _mm_or_si128(diff_img12, diff_img21); // Safe because one is zero see http://0x80.pl/notesen/2018-03-11-sse-abs-unsigned.html
            _mm_storeu_si128(reinterpret_cast<__m128i*>(out_img_row + col), diff);
        }
    }

	// Stop the timer
	auto finish_time = get_current_time();

    cv::imwrite(utils::paths::PARALLEL_OUTPUT_PATH, output_image);

    release_image(output_image);

	return calculate_duration(start_time, finish_time);
}
