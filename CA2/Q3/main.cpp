#include <string>
#include <vector>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <sys/time.h>
#include <cstdlib>
#include <x86intrin.h>
#include <tmmintrin.h>

using namespace cv;

int main()
{
    struct timeval start, end;
    std::string first_frame = "./assets/1.png";
    std::string second_frame = "./assets/2.png";

    // Load frames
    Mat img1 = imread(first_frame, IMREAD_ANYCOLOR);
    Mat img2 = imread(second_frame, IMREAD_ANYCOLOR);
    if(img1.size() != img2.size())
    {
        printf("Illegeal frames\n");
        exit(EXIT_FAILURE);
    }
    const unsigned NCOLS = img1.cols;
    const unsigned NROWS = img2.rows;

    // Sequential
    cv::Mat seq_res(NROWS, NCOLS, CV_8U);

    // Define pointers
    uchar* seq_res_ptr = seq_res.data;
    uchar* img1_ptr = img1.data;
    uchar* img2_ptr = img2.data;


    gettimeofday(&start, NULL);
    for(int i = 0; i < NROWS; i++)
    {
        for(int j = 0; j < NCOLS; j++)
        {
            *(seq_res_ptr + i * NCOLS + j) = abs(
                *(img1_ptr + i * NCOLS + j) - *(img2_ptr + i * NCOLS +j)
                );
        }
    }
    gettimeofday(&end, NULL);
    printf("Sequential Result:\n\t"
           "-%ld.%ld seconds\n",
           end.tv_sec-start.tv_sec,
           end.tv_usec-start.tv_usec);
    cv::namedWindow("Sequential", cv::WINDOW_AUTOSIZE);
    imshow("Sequential", seq_res);

    return 0;
}