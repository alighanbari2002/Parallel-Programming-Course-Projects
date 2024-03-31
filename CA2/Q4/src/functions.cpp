#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#ifdef _WIN32
#include <intrin.h>
#else
#include <x86intrin.h>
#endif

#include "functions.hpp"
#include "utils.hpp"

long long image_blending_serial(const cv::Mat& front, const cv::Mat& logo)
{
    cv::Mat output_image = front.clone();

    uchar* out_img_row;

    int row, col, new_pixel;

	// Start the timer
	auto start_time = get_current_time();

    for (row = 0; row < logo.rows; ++row)
    {
        const uchar* front_row = front.ptr<uchar>(row);
        const uchar* logo_row = logo.ptr<uchar>(row);
        out_img_row = output_image.ptr<uchar>(row);

        for (col = 0; col < logo.cols; ++col)
        {
            new_pixel = static_cast<int>(front_row[col] + utils::config::ALPHA * logo_row[col]);
            out_img_row[col] = new_pixel > 255 ? 255 : static_cast<uchar>(new_pixel);
        }
    }

    // Stop the timer
	auto finish_time = get_current_time();

    cv::imwrite(utils::paths::SERIAL_OUTPUT_PATH, output_image);

    release_image(output_image);

	return calculate_duration(start_time, finish_time);
}

long long image_blending_parallel(const cv::Mat& front, const cv::Mat& logo)
{
    cv::Mat output_image = front.clone();
    
    __m128i_u logo_chunk, front_chunk;
    __m128i_u* output_chunk;

    const int logo_width = logo.cols - utils::simd::GRAY_INTERVAL;

    uchar* out_img_row;

    int row, col;

	// Start the timer
	auto start_time = get_current_time();

    for (row = 0; row < logo.rows; ++row)
    {
        const uchar* front_row = front.ptr<uchar>(row);
        const uchar* logo_row = logo.ptr<uchar>(row);
        out_img_row = output_image.ptr<uchar>(row);

        for (col = 0; col < logo_width; col += utils::simd::GRAY_INTERVAL)
        {
            front_chunk = _mm_lddqu_si128(reinterpret_cast<const __m128i_u*>(front_row + col));
            logo_chunk  = _mm_lddqu_si128(reinterpret_cast<const __m128i_u*>(logo_row + col));
            output_chunk = reinterpret_cast<__m128i_u*>(out_img_row + col);

            // Apply alpha blending to logo chunk
            logo_chunk = _mm_srli_epi16(logo_chunk, 2);
            logo_chunk = _mm_and_si128(logo_chunk, utils::simd::MASK_DIVIDE_BY_4);

            // Store blended result
            _mm_storeu_si128(output_chunk, _mm_adds_epu8(logo_chunk, front_chunk));
        }
    }

	// Stop the timer
	auto finish_time = get_current_time();

    cv::imwrite(utils::paths::PARALLEL_OUTPUT_PATH, output_image);

    release_image(output_image);

	return calculate_duration(start_time, finish_time);
}
