#include <iostream>
#include <sstream>
#include <cstdio>
#include <cstdlib>
#include <omp.h>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>

#define FRONT_IAMGE_PATH "../assets/front.png"
#define LOGO_IMAGE_PATH  "../assets/logo.png"
#define OUTPUT_DIR       "../output/"

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

bool can_fit_inside(const cv::Size& inner_size, const cv::Size& outer_size)
{
    return inner_size.width <= outer_size.width && inner_size.height <= outer_size.height;
}

double get_current_time()
{
    return omp_get_wtime();
}

long long calculate_duration(const double& start_time, const double& finish_time)
{
    return static_cast<long long>((finish_time - start_time) * 1e9);
}

const char* format_time(const long long& time_ns)
{
    std::stringstream time_formatter;
    time_formatter.imbue(std::locale(""));
    time_formatter << time_ns;
    return time_formatter.str().c_str();
}

long long image_blending_serial(const cv::Mat& front, const cv::Mat& logo, const float& alpha)
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
            new_pixel = static_cast<int>(front_row[col] + alpha * logo_row[col]);
            out_img_row[col] = new_pixel > 255 ? 255 : static_cast<uchar>(new_pixel);
        }
    }

    // Stop the timer
	double finish_time = get_current_time();

    cv::imwrite(OUTPUT_DIR "serial output.png", output_image);

    release_image(output_image);

	return calculate_duration(start_time, finish_time);
}

long long image_blending_parallel(const cv::Mat& front, const cv::Mat& logo, const float& alpha)
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
                new_pixel = static_cast<int>(front_row[col] + alpha * logo_row[col]);
                out_img_row[col] = new_pixel > 255 ? 255 : static_cast<uchar>(new_pixel);
            }
        }

    // Stop the timer
	double finish_time = get_current_time();

    cv::imwrite(OUTPUT_DIR "parallel output.png", output_image);

    release_image(output_image);

	return calculate_duration(start_time, finish_time);
}

int main()
{
	print_group_info();

    cv::Mat front_image = load_image(FRONT_IAMGE_PATH);
    cv::Mat logo_image = load_image(LOGO_IMAGE_PATH);

    CV_Assert(can_fit_inside(logo_image.size(), front_image.size()));

    const float ALPHA = 0.25f;

	// Set the number of threads
	omp_set_num_threads(2);

    long long elapsed_time_serial = image_blending_serial(front_image, logo_image, ALPHA);
	long long elapsed_time_parallel = image_blending_parallel(front_image, logo_image, ALPHA);

    release_image(front_image);
    release_image(logo_image);

    printf("\nRun Time (ns):\n");
    printf("\t- Serial Method: %s\n", format_time(elapsed_time_serial));
    printf("\t- Parallel Method: %s\n", format_time(elapsed_time_parallel));
    printf("\nSpeedup: %.4lf\n", static_cast<double>(elapsed_time_serial) / elapsed_time_parallel);
    
	return EXIT_SUCCESS;
}
