#include <omp.h>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>

#include "functions.hpp"
#include "utils.hpp"

long long image_blending_serial(const cv::Mat& front, const cv::Mat& logo)
{
    cv::Mat output_image = front.clone();

    uchar* out_img_row;

    int row, col, new_pixel;

	// Start the timer
	double start_time = get_current_time();

    for (row = 0; row < logo.rows; ++row)
    {
        const uchar* front_row = front.ptr<uchar>(row);
        const uchar* logo_row = logo.ptr<uchar>(row);
        out_img_row = output_image.ptr<uchar>(row);

        for (col = 0; col < logo.cols; ++col)
        {
            new_pixel = static_cast<int>(front_row[col] + utils::config::ALPHA * logo_row[col]);
            out_img_row[col] = new_pixel > 255 ? 255 : static_cast<uchar>(new_pixel);
        }
    }

    // Stop the timer
	double finish_time = get_current_time();

    cv::imwrite(utils::paths::SERIAL_OUTPUT_PATH, output_image);

    release_image(output_image);

	return calculate_duration(start_time, finish_time);
}

long long image_blending_parallel(const cv::Mat& front, const cv::Mat& logo)
{
    cv::Mat output_image = front.clone();

    uchar* out_img_row;

    int row, col, new_pixel;

	// Start the timer
	double start_time = get_current_time();

    #pragma omp parallel for simd default(shared) private(out_img_row, row, col, new_pixel) schedule(static)
        for (row = 0; row < logo.rows; ++row)
        {
            const uchar* front_row = front.ptr<uchar>(row);
            const uchar* logo_row = logo.ptr<uchar>(row);
            out_img_row = output_image.ptr<uchar>(row);

            for (col = 0; col < logo.cols; ++col)
            {
                new_pixel = static_cast<int>(front_row[col] + utils::config::ALPHA * logo_row[col]);
                out_img_row[col] = new_pixel > 255 ? 255 : static_cast<uchar>(new_pixel);
            }
        }

    // Stop the timer
	double finish_time = get_current_time();

    cv::imwrite(utils::paths::PARALLEL_OUTPUT_PATH, output_image);

    release_image(output_image);

	return calculate_duration(start_time, finish_time);
}
