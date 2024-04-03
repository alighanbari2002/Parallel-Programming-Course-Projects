#ifndef UTILS_HPP
#define UTILS_HPP

#include <cstddef>
#include <string>
#include <chrono>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>

typedef struct {
    const cv::Mat* img1;
    const cv::Mat* img2;
    cv::Mat* out_img;
    int start_row;
    int end_row;
} thread_data_t;

namespace utils {

    namespace paths {
        constexpr char FIRST_IMAGE_PATH[]     = "../assets/image_01.png";
        constexpr char SECOND_IMAGE_PATH[]    = "../assets/image_02.png";
        constexpr char SERIAL_OUTPUT_PATH[]   = "../output/serial_output.png";
        constexpr char PARALLEL_OUTPUT_PATH[] = "../output/parallel_output.png";
    } // namespace paths

} // namespace utils

void print_group_info();

cv::Mat load_image(const char* const &path);

void release_image(cv::Mat& img);

std::chrono::high_resolution_clock::time_point get_current_time();

template <typename T>
long long calculate_duration(const T& start_time, const T& finish_time)
{
    return std::chrono::duration_cast<std::chrono::nanoseconds>(finish_time - start_time).count();
}

std::string format_time(const long long& time_ns);

#endif // UTILS_HPP
