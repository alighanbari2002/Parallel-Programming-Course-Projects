#include <cstdio>
#include <cstddef>
#include <chrono>
#include <sstream>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>

#include "utils.hpp"

void print_group_info()
{
    printf("Group Members:\n");
	printf("\t- Ali Ghanbari [810199473]\n");
	printf("\t- Behrad Elmi  [810199557]\n");
}

cv::Mat load_image(const char* const &path)
{
    cv::Mat img = cv::imread(path, cv::IMREAD_GRAYSCALE);
    CV_Assert(!img.empty());
    return img;
}

void release_image(cv::Mat& img)
{
    img.release();
}

std::chrono::high_resolution_clock::time_point get_current_time()
{
	return std::chrono::high_resolution_clock::now();
}

const char* format_time(const long long& time_ns)
{
    std::stringstream time_formatter;
    time_formatter.imbue(std::locale(""));
    time_formatter << time_ns;
    return time_formatter.str().c_str();
}
