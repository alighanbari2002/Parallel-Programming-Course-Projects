#ifndef FUNCTIONS_HPP
#define FUNCTIONS_HPP

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>

long long calculate_absolute_difference_serial(const cv::Mat& img1, const cv::Mat& img2);

long long calculate_absolute_difference_parallel(const cv::Mat& img1, const cv::Mat& img2);

#endif // FUNCTIONS_HPP
