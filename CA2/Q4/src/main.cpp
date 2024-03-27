#include <iostream>
#include <sstream>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <chrono>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#ifdef 		_WIN32
#include <intrin.h>
#else
#include <x86intrin.h>
#endif

#define FRONT_IAMGE_PATH "../assets/front.png"
#define LOGO_IMAGE_PATH  "../assets/logo.png"
#define OUTPUT_DIR       "../output/"

void print_group_info()
{
    printf("Group Members:\n");
	printf("\t- Ali Ghanbari [810199473]\n");
	printf("\t- Behrad Elmi  [810199557]\n");
}

cv::Mat load_image(const char* const &path)
{
    cv::Mat img = cv::imread(path, cv::IMREAD_GRAYSCALE);
    CV_Assert(!img.empty());
    return img;
}

void release_image(cv::Mat& img)
{
    img.release();
}

bool can_fit_inside(const cv::Size& inner_size, const cv::Size& outer_size)
{
    return inner_size.width <= outer_size.width && inner_size.height <= outer_size.height;
}

std::chrono::high_resolution_clock::time_point get_current_time()
{
	return std::chrono::high_resolution_clock::now();
}

template <typename T>
long long calculate_duration(const T& start_time, const T& finish_time)
{
    return std::chrono::duration_cast<std::chrono::nanoseconds>(finish_time - start_time).count();
}

const char* format_time(const long long& time_ns)
{
    std::stringstream time_formatter;
    time_formatter.imbue(std::locale(""));
    time_formatter << time_ns;
    return time_formatter.str().c_str();
}

long long image_blending_serial(const cv::Mat& front, const cv::Mat& logo, const float& alpha)
{
    cv::Mat output_image = front.clone();

    const uchar* front_row;
    const uchar* logo_row;
    uchar* out_img_row;

    int row, col, new_pixel;

	// Start the timer
	auto start_time = get_current_time();

    for (row = 0; row < logo.rows; ++row)
    {
        front_row = front.ptr<uchar>(row);
        logo_row = logo.ptr<uchar>(row);
        out_img_row = output_image.ptr<uchar>(row);

        for (col = 0; col < logo.cols; ++col)
        {
            new_pixel = front_row[col] + alpha * logo_row[col];
            out_img_row[col] = new_pixel > 255 ? 255 : static_cast<uchar>(new_pixel);
        }
    }

    // Stop the timer
	auto finish_time = get_current_time();

    cv::imwrite(OUTPUT_DIR "serial output.png", output_image);

    release_image(output_image);

	return calculate_duration(start_time, finish_time);
}

long long image_blending_parallel(const cv::Mat& front, const cv::Mat& logo, const float& alpha)
{
    cv::Mat output_image = front.clone();
    
    __m128i_u logo_chunk, front_chunk;
    __m128i_u* output_chunk;

    const int M128_GRAY_INTERVAL = 16;
    const int logo_width = logo.cols - M128_GRAY_INTERVAL;
    // Divide by 4 
    // 00 --> xxxx xxxx xxxx xx --> xx
    // 00xx xxxx xxxx xxxx --> 00xx xxxx 00xx xxxx
    const __m128i_u mask_divide_by_4 = _mm_set1_epi16(0xFF3F);

    const uchar* front_row;
    const uchar* logo_row;
    uchar* out_img_row;

    int row, col;

	// Start the timer
	auto start_time = get_current_time();

    for (row = 0; row < logo.rows; ++row)
    {
        front_row = front.ptr<uchar>(row);
        logo_row = logo.ptr<uchar>(row);
        out_img_row = output_image.ptr<uchar>(row);

        for (col = 0; col < logo_width; col += M128_GRAY_INTERVAL)
        {
            front_chunk = _mm_lddqu_si128(reinterpret_cast<const __m128i_u*>(front_row + col));
            logo_chunk  = _mm_lddqu_si128(reinterpret_cast<const __m128i_u*>(logo_row + col));
            output_chunk = reinterpret_cast<__m128i_u*>(out_img_row + col);

            // Apply alpha blending to logo chunk
            logo_chunk = _mm_srli_epi16(logo_chunk, 2);
            logo_chunk = _mm_and_si128(logo_chunk, mask_divide_by_4);

            // Store blended result
            _mm_storeu_si128(output_chunk, _mm_adds_epu8(logo_chunk, front_chunk));
        }
    }

	// Stop the timer
	auto finish_time = get_current_time();

    cv::imwrite(OUTPUT_DIR "parallel output.png", output_image);

    release_image(output_image);

	return calculate_duration(start_time, finish_time);
}

int main()
{
	print_group_info();

    cv::Mat front_image = load_image(FRONT_IAMGE_PATH);
    cv::Mat logo_image = load_image(LOGO_IMAGE_PATH);

    CV_Assert(can_fit_inside(logo_image.size(), front_image.size()));

    const float ALPHA = 0.25f;

    long long elapsed_time_serial = image_blending_serial(front_image, logo_image, ALPHA);
	long long elapsed_time_parallel = image_blending_parallel(front_image, logo_image, ALPHA);

    release_image(front_image);
    release_image(logo_image);

    printf("\nRun Time (ns):\n");
    printf("\t- Serial Method: %s\n", format_time(elapsed_time_serial));
    printf("\t- Parallel Method: %s\n", format_time(elapsed_time_parallel));
    printf("\nSpeedup: %.4lf\n", static_cast<double>(elapsed_time_serial) / elapsed_time_parallel);
    
	return EXIT_SUCCESS;
}
