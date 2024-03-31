#include <omp.h>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>

#include "functions.hpp"
#include "utils.hpp"

long long calculate_absolute_difference_serial(const cv::Mat& img1, const cv::Mat& img2)
{
    cv::Mat output_image(img1.size(), img1.type());

    uchar* out_img_row;

    int row, col;

	// Start the timer
	double start_time = get_current_time();

    for (row = 0; row < output_image.rows; ++row)
    {
        const uchar* img1_row = img1.ptr<uchar>(row);
        const uchar* img2_row = img2.ptr<uchar>(row);
        out_img_row = output_image.ptr<uchar>(row);
        
        for (col = 0; col < output_image.cols; ++col)
        {
            out_img_row[col] = static_cast<uchar>(abs(img1_row[col] - img2_row[col]));
        }
    }

    // Stop the timer
	double finish_time = get_current_time();

    cv::imwrite(utils::paths::SERIAL_OUTPUT_PATH, output_image);

    release_image(output_image);

	return calculate_duration(start_time, finish_time);
}

long long calculate_absolute_difference_parallel(const cv::Mat& img1, const cv::Mat& img2)
{
    cv::Mat output_image(img1.size(), img1.type());

    uchar* out_img_row;

    int row, col;

	// Start the timer
	double start_time = get_current_time();

    #pragma omp parallel for simd default(shared) private(out_img_row, row, col) schedule(static)
        for (row = 0; row < output_image.rows; ++row)
        {
            const uchar* img1_row = img1.ptr<uchar>(row);
            const uchar* img2_row = img2.ptr<uchar>(row);
            out_img_row = output_image.ptr<uchar>(row);
            
            for (col = 0; col < output_image.cols; ++col)
            {
                out_img_row[col] = static_cast<uchar>(abs(img1_row[col] - img2_row[col]));
            }
        }

    // Stop the timer
	double finish_time = get_current_time();

    cv::imwrite(utils::paths::PARALLEL_OUTPUT_PATH, output_image);

    release_image(output_image);

	return calculate_duration(start_time, finish_time);
}
