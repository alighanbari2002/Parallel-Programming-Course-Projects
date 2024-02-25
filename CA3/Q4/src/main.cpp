#include <stdlib.h>
#include <stdio.h>
#include <omp.h>
#include <sstream>
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
#define ALPHA 0.25
#define M128_GRAY_INTERVAL 16

// Global variables
Mat logo;
Mat front;
unsigned int LOGO_ROW;
unsigned int LOGO_COL;
unsigned int FRONT_ROW;
unsigned int FRONT_COL;

ll serial_implementation()
{
    Mat out_img_serial(FRONT_ROW, FRONT_COL, CV_8U);
    size_t row, col;

	// Start the timer
	double start = omp_get_wtime();

    for(row = 0; row < FRONT_ROW; ++row)
    {
        for(col = 0; col < FRONT_COL; ++col)
        {
            if(row <= LOGO_ROW && col <= LOGO_COL)
            {
                if(front.at<uchar> (row, col) + ALPHA * logo.at<uchar> (row, col) > 255)
                {
                    out_img_serial.at<uchar> (row, col) = 255;
                }
                else
                {
                    out_img_serial.at<uchar> (row, col) = front.at<uchar> (row, col) + ALPHA * logo.at<uchar> (row, col);
                }
            }
            else
            {
                out_img_serial.at<uchar> (row, col) = front.at<uchar> (row, col);
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

ll parallel_implementation()
{
    Mat out_img_parallel(FRONT_ROW, FRONT_COL, CV_8U);
    size_t row, col;

	// Start the timer
	double start = omp_get_wtime();

    #pragma omp parallel for simd simdlen(16) default(shared) private(row, col) schedule(auto)
        for(row = 0; row < FRONT_ROW; ++row)
        {
            for(col = 0; col < FRONT_COL; ++col)
            {
                if(row <= LOGO_ROW && col <= LOGO_COL)
                {
                    if(front.at<uchar> (row, col) + ALPHA * logo.at<uchar> (row, col) > 255)
                    {
                        out_img_parallel.at<uchar> (row, col) = 255;
                    }
                    else
                    {
                        out_img_parallel.at<uchar> (row, col) = front.at<uchar> (row, col) + ALPHA * logo.at<uchar> (row, col);
                    }
                }
                else
                {
                    out_img_parallel.at<uchar> (row, col) = front.at<uchar> (row, col);
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

    // Load frames
    logo  = imread(LOGO_IMAGE, IMREAD_GRAYSCALE);
    front = imread(FRONT_IAMGE, IMREAD_GRAYSCALE);
    if (logo.size().width > front.size().width ||
        logo.size().height > front.size().height)
    {
        printf("Illegal frames!\n");
        exit(EXIT_FAILURE);
    }

    LOGO_ROW  = logo.rows;
    LOGO_COL  = logo.cols;
    FRONT_ROW = front.rows;
    FRONT_COL = front.cols;

	int num_threads = omp_get_max_threads() - 1;
	omp_set_num_threads(num_threads);

    printf("\nRun Time (ns):\n");
    ll serial_time = serial_implementation();
	ll parallel_time = parallel_implementation();

	printf("\nSpeedup: %.4lf\n", (double)serial_time / (double)parallel_time);

    logo.release();
    front.release();

    return 0;
}
