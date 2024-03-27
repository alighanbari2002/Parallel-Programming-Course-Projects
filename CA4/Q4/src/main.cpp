#include <iostream>
#include <sstream>
#include <cstdio>
#include <cstdlib>
#include <chrono>
#include <unistd.h>
#include <pthread.h>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>

typedef struct {
    const cv::Mat* front;
    const cv::Mat* logo;
    cv::Mat* out_img;
    float alpha;
    int start_row;
    int end_row;
} thread_data_t;

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

std::chrono::high_resolution_clock::time_point get_current_time()
{
	return std::chrono::high_resolution_clock::now();
}

template <typename T>
long long calculate_duration(const T& start_time, const T& finish_time)
{
    return std::chrono::duration_cast<std::chrono::nanoseconds>(finish_time - start_time).count();
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

	// Start the timer
	auto start_time = get_current_time();

    const uchar* front_row;
    const uchar* logo_row;
    uchar* out_img_row;

    int row, col, new_pixel;

    for (row = 0; row < logo.rows; ++row)
    {
        front_row = front.ptr<uchar>(row);
        logo_row = logo.ptr<uchar>(row);
        out_img_row = output_image.ptr<uchar>(row);

        for (col = 0; col < logo.cols; ++col)
        {
            new_pixel = front_row[col] + alpha * logo_row[col];
            out_img_row[col] = new_pixel > 255 ? 255 : static_cast<uchar>(new_pixel);
        }
    }

    // Stop the timer
	auto finish_time = get_current_time();

    cv::imwrite(OUTPUT_DIR "serial output.png", output_image);

    release_image(output_image);

	return calculate_duration(start_time, finish_time);
}

void* blend_images_with_alpha(void* arg)
{
    thread_data_t* data = (thread_data_t*) arg;

    const uchar* front_row;
    const uchar* logo_row;
    uchar* out_img_row;

    int row, col, new_pixel;

    for (row = data->start_row; row < data->end_row; ++row)
    {
        front_row = data->front->ptr<uchar>(row);
        logo_row = data->logo->ptr<uchar>(row);
        out_img_row = data->out_img->ptr<uchar>(row);

        for (col = 0; col < data->logo->cols; ++col)
        {            
            new_pixel = front_row[col] + data->alpha * logo_row[col];
            out_img_row[col] = new_pixel > 255 ? 255 : static_cast<uchar>(new_pixel);
        }
    }

    pthread_exit(nullptr);
}

long long image_blending_parallel(const cv::Mat& front, const cv::Mat& logo, const float& alpha, const size_t& num_threads)
{
    cv::Mat output_image = front.clone();

    pthread_t threads[num_threads];
    thread_data_t thread_args[num_threads];

    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    int rows_per_thread = logo.rows / num_threads;

    size_t i;

	// Start the timer
	auto start_time = get_current_time();

    // Assign the arguments for each thread
    for (i = 0; i < num_threads; ++i)
    {
        thread_args[i].front = &front;
        thread_args[i].logo = &logo;
        thread_args[i].out_img = &output_image;

        thread_args[i].alpha = alpha;

        thread_args[i].start_row = i * rows_per_thread;
        thread_args[i].end_row = (i == num_threads - 1) ? logo.rows : (i + 1) * rows_per_thread;

        int rc = pthread_create(&threads[i], &attr, blend_images_with_alpha, static_cast<void*>(&thread_args[i]));
        if (rc)
        {
            printf("ERROR: return code from pthread_create() is %d\n", rc);
            exit(EXIT_FAILURE);
        }
    }

    // Free the attribute and join the threads
    pthread_attr_destroy(&attr);
    for (i = 0; i < num_threads; ++i)
    {
        int rc = pthread_join(threads[i], NULL);
        if (rc)
        {
            printf("ERROR: return code from pthread_join() is %d\n", rc);
            exit(EXIT_FAILURE);
        }
    }

	// Stop the timer
	auto finish_time = get_current_time();

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
    size_t num_threads = 3;

    long long elapsed_time_serial = image_blending_serial(front_image, logo_image, ALPHA);
	long long elapsed_time_parallel = image_blending_parallel(front_image, logo_image, ALPHA, num_threads);

    release_image(front_image);
    release_image(logo_image);

    printf("\nRun Time (ns):\n");
    printf("\t- Serial Method: %s\n", format_time(elapsed_time_serial));
    printf("\t- Parallel Method: %s\n", format_time(elapsed_time_parallel));
    printf("\nSpeedup: %.4lf\n", static_cast<double>(elapsed_time_serial) / elapsed_time_parallel);
    
    pthread_exit(nullptr);
}
