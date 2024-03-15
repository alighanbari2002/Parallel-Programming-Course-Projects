#include <stdlib.h>
#include <stdio.h>
#include <omp.h>
#include <sstream>
#include <cassert>
#include <type_traits>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>

using cv::Mat;
using cv::imwrite;
using cv::imread;
using cv::IMREAD_GRAYSCALE;
using std::stringstream;
using std::locale;

typedef long long ll;

#define FRONT_IAMGE "../assets/front.png"
#define LOGO_IMAGE  "../assets/logo.png"
#define OUTPUT_DIR  "../output/"

const double ALPHA = 0.25;
const int M128_GRAY_INTERVAL = 16;

ll blend_images_serial(const Mat& front, const Mat& logo, double alpha)
{
    Mat out_img_serial = front.clone();

    int row, col;

	// Start the timer
	double start = omp_get_wtime();

    for(row = 0; row < out_img_serial.rows; ++row)
    {
        const uchar* front_row = front.ptr<uchar>(row);
        const uchar* logo_row = logo.ptr<uchar>(row);
        uchar* out_img_row = out_img_serial.ptr<uchar>(row);

        for(col = 0; col < out_img_serial.cols; ++col)
        {
            if(row < logo.rows && col < logo.cols)
            {
                int new_pixel = front_row[col] + alpha * logo_row[col];
                out_img_row[col] = new_pixel > 255 ? 255 : static_cast<uchar>(new_pixel);
            }
        }
    }

	// Stop the timer
	double finish = omp_get_wtime();

    imwrite(OUTPUT_DIR "serial output.png", out_img_serial);
    out_img_serial.release();

	ll execution_time = (finish - start) * pow(10, 9);

	// Use a string stream to format the output
	stringstream ss;
	ss.imbue(locale(""));
	ss << execution_time;

    printf("\t- Serial Method: %s\n", ss.str().c_str());

    return execution_time;
}

ll blend_images_parallel(const Mat& front, const Mat& logo, double alpha)
{
    Mat out_img_parallel = front.clone();

    int row, col;

	int num_threads = omp_get_max_threads() - 1;
	omp_set_num_threads(num_threads);

	// Start the timer
	double start = omp_get_wtime();

    #pragma omp parallel for simd default(shared) private(row, col) schedule(auto)
        for(row = 0; row < out_img_parallel.rows; ++row)
        {
            const uchar* front_row = front.ptr<uchar>(row);
            const uchar* logo_row = logo.ptr<uchar>(row);
            uchar* out_img_row = out_img_parallel.ptr<uchar>(row);

            for(col = 0; col < out_img_parallel.cols; ++col)
            {
                if(row < logo.rows && col < logo.cols)
                {
                    int new_pixel = front_row[col] + alpha * logo_row[col];
                    out_img_row[col] = new_pixel > 255 ? 255 : static_cast<uchar>(new_pixel);
                }
            }
        }

	// Stop the timer
	double finish = omp_get_wtime();

    imwrite(OUTPUT_DIR "parallel output.png", out_img_parallel);
    out_img_parallel.release();

	ll execution_time = (finish - start) * pow(10, 9);

	// Use a string stream to format the output
	stringstream ss;
	ss.imbue(locale(""));
	ss << execution_time;

    printf("\t- Parallel Method: %s\n", ss.str().c_str());

    return execution_time;
}

void print_group_info()
{
    printf("Group Members:\n");
	printf("\t- Ali Ghanbari [810199473]\n");
	printf("\t- Behrad Elmi  [810199557]\n");
}

int main()
{
	print_group_info();

    Mat front_image = imread(FRONT_IAMGE, IMREAD_GRAYSCALE);
    Mat logo_image = imread(LOGO_IMAGE, IMREAD_GRAYSCALE);

    CV_Assert(logo_image.size().width <= front_image.size().width && 
              logo_image.size().height <= front_image.size().height && 
              "Illegal frames: logo_image is larger than front_image");

    printf("\nRun Time (ns):\n");
    ll serial_time = blend_images_serial(front_image, logo_image, ALPHA);
	ll parallel_time = blend_images_parallel(front_image, logo_image, ALPHA);

	printf("\nSpeedup: %.4lf\n", (double) serial_time / (double) parallel_time);

    front_image.release();
    logo_image.release();

    return 0;
}
