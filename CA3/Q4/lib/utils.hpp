#ifndef UTILS_HPP
#define UTILS_HPP

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

} // namespace utils

void print_group_info();

cv::Mat load_image(const char* const &path);

void release_image(cv::Mat& img);

bool can_fit_inside(const cv::Size& inner_size, const cv::Size& outer_size);

double get_current_time();

long long calculate_duration(const double& start_time, const double& finish_time);

std::string format_time(const long long& time_ns);

#endif // UTILS_HPP
