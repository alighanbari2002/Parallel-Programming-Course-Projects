#include <cstdio>
#include <cstdlib>
#include <string>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>

#include "functions.hpp"
#include "utils.hpp"

int main()
{
	print_group_info();

    cv::Mat first_image = load_image(utils::paths::FIRST_IMAGE_PATH);
    cv::Mat second_image = load_image(utils::paths::SECOND_IMAGE_PATH);

    CV_Assert(first_image.size() == second_image.size() && first_image.type() == second_image.type());

    long long elapsed_time_serial = calculate_absolute_difference_serial(first_image, second_image);
	long long elapsed_time_parallel = calculate_absolute_difference_parallel(first_image, second_image);

    release_image(first_image);
    release_image(second_image);

    printf("\nRun Time (ns):\n");
    printf("\t- Serial Method: %s\n", format_time(elapsed_time_serial).c_str());
    printf("\t- Parallel Method: %s\n", format_time(elapsed_time_parallel).c_str());
    printf("\nSpeedup: %.4lf\n", static_cast<double>(elapsed_time_serial) / elapsed_time_parallel);
    
	return EXIT_SUCCESS;
}
