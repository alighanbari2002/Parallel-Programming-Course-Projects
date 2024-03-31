#ifndef FUNCTIONS_HPP
#define FUNCTIONS_HPP

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>

long long image_blending_serial(const cv::Mat& front, const cv::Mat& logo);

long long image_blending_parallel(const cv::Mat& front, const cv::Mat& logo);

#endif // FUNCTIONS_HPP
