#include <iostream>
#include <sstream>
#include <cstdio>
#include <cstdlib>
#include <omp.h>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>

#define IMAGE_01_PATH "../assets/image_01.png"
#define IMAGE_02_PATH "../assets/image_02.png"
#define OUTPUT_DIR    "../output/"

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

    cv::imwrite(OUTPUT_DIR "serial output.png", output_image);

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

    cv::imwrite(OUTPUT_DIR "parallel output.png", output_image);

    release_image(output_image);

	return calculate_duration(start_time, finish_time);
}

int main()
{
	print_group_info();

    cv::Mat image_01 = load_image(IMAGE_01_PATH);
    cv::Mat image_02 = load_image(IMAGE_02_PATH);

    CV_Assert(image_01.size() == image_02.size() && image_01.type() == image_02.type());

	// Set the number of threads
	size_t num_threads = omp_get_max_threads() - 1;
	omp_set_num_threads(num_threads);

    long long elapsed_time_serial = calculate_absolute_difference_serial(image_01, image_02);
    long long elapsed_time_parallel = calculate_absolute_difference_parallel(image_01, image_02);

    release_image(image_01);
    release_image(image_02);

    printf("\nRun Time (ns):\n");
    printf("\t- Serial Method: %s\n", format_time(elapsed_time_serial));
    printf("\t- Parallel Method: %s\n", format_time(elapsed_time_parallel));
    printf("\nSpeedup: %.4lf\n", static_cast<double>(elapsed_time_serial) / elapsed_time_parallel);
    
	return EXIT_SUCCESS;
}
