#include <stdio.h>
#include <stdlib.h>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <unistd.h>
#include <chrono>
#include <pthread.h>

using cv::Mat;
using cv::imwrite;
using cv::imread;
using cv::IMREAD_GRAYSCALE;
using std::chrono::high_resolution_clock;
using std::chrono::duration_cast;
using std::chrono::nanoseconds;

#define IMAGE_01   "../assets/image_01.png"
#define IMAGE_02   "../assets/image_02.png"
#define OUTPUT_DIR "../output/"
#define NUM_THREADS static_cast<size_t>(sysconf(_SC_NPROCESSORS_ONLN) - 1)

