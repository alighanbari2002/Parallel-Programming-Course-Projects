#include <cstdio>
#include <cstdlib>
#include <thread>
#include <algorithm>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>

#include "functions.hpp"
#include "utils.hpp"

int main()
{
	print_group_info();

    cv::Mat front_image = load_image(utils::paths::FRONT_IAMGE_PATH);
    cv::Mat logo_image = load_image(utils::paths::LOGO_IMAGE_PATH);

    CV_Assert(can_fit_inside(logo_image.size(), front_image.size()));

	// Set the number of threads
    size_t num_threads = std::min<size_t>(3, std::thread::hardware_concurrency());

    long long elapsed_time_serial = image_blending_serial(front_image, logo_image);
	long long elapsed_time_parallel = image_blending_parallel(front_image, logo_image, num_threads);

    release_image(front_image);
    release_image(logo_image);

    printf("\nRun Time (ns):\n");
    printf("\t- Serial Method: %s\n", format_time(elapsed_time_serial));
    printf("\t- Parallel Method: %s\n", format_time(elapsed_time_parallel));
    printf("\nSpeedup: %.4lf\n", static_cast<double>(elapsed_time_serial) / elapsed_time_parallel);
    
    pthread_exit(nullptr);
}
