#ifndef UTILS_HPP
#define UTILS_HPP

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>

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

double get_current_time();

long long calculate_duration(const double& start_time, const double& finish_time);

const char* format_time(const long long& time_ns);

#endif // UTILS_HPP
