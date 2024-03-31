#ifndef FUNCTIONS_HPP
#define FUNCTIONS_HPP

#include <cstddef>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>

long long calculate_absolute_difference_serial(const cv::Mat& img1, const cv::Mat& img2);

void* process_image_rows(void* arg);

long long calculate_absolute_difference_parallel(const cv::Mat& img1, const cv::Mat& img2, const size_t& num_threads);

#endif // FUNCTIONS_HPP
