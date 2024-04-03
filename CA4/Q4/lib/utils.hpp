#ifndef UTILS_HPP
#define UTILS_HPP

#include <cstddef>
#include <string>
#include <chrono>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>

typedef struct {
    const cv::Mat* front;
    const cv::Mat* logo;
    cv::Mat* out_img;
    int start_row;
    int end_row;
} thread_data_t;

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
