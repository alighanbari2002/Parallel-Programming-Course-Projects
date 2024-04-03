#ifndef UTILS_HPP
#define UTILS_HPP

#include <chrono>
#include <string>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>

namespace utils {

    namespace config {
        constexpr float ALPHA = 0.25f;
    } // namespace config

    namespace paths {
        constexpr char LOGO_IMAGE_PATH[]      = "../assets/logo.png";
        constexpr char FRONT_IAMGE_PATH[]     = "../assets/front.png";
        constexpr char SERIAL_OUTPUT_PATH[]   = "../output/serial_output.png";
        constexpr char PARALLEL_OUTPUT_PATH[] = "../output/parallel_output.png";
    } // namespace paths

    namespace simd {
        constexpr int GRAY_INTERVAL = 16; // Interval used for grayscale operations in SIMD

        // Mask for dividing packed 16-bit integers by 4 using SIMD operations.
        // The mask sets the two least significant bits of each byte to 0,
        // effectively performing a bitwise right shift by 2 positions.
        const __m128i_u MASK_DIVIDE_BY_4 = _mm_set1_epi16(static_cast<short int>(0xFF3F));
    } // namespace simd

} // namespace utils

void print_group_info();

cv::Mat load_image(const char* const &path);

void release_image(cv::Mat& img);

bool can_fit_inside(const cv::Size& inner_size, const cv::Size& outer_size);

std::chrono::high_resolution_clock::time_point get_current_time();

template <typename T>
long long calculate_duration(const T& start_time, const T& finish_time)
{
    return std::chrono::duration_cast<std::chrono::nanoseconds>(finish_time - start_time).count();
}

std::string format_time(const long long& time_ns);

#endif // UTILS_HPP
