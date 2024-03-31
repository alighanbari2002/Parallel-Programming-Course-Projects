#ifndef FUNCTIONS_HPP
#define FUNCTIONS_HPP

#include <cstddef>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>

long long image_blending_serial(const cv::Mat& front, const cv::Mat& logo);

void* blend_images_with_alpha(void* arg);

long long image_blending_parallel(const cv::Mat& front, const cv::Mat& logo, const size_t& num_threads);

#endif // FUNCTIONS_HPP
