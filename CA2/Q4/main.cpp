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
    std::string logo = "./assets/logo.png";
    std::string front = "./assets/front.png";

    Mat limg = imread(logo, IMREAD_ANYCOLOR);
    Mat fimg = imread(front, IMREAD_ANYCOLOR);
    const int HIGH_COL = limg.cols > fimg.cols ? limg.cols : fimg.cols;
    const int HIGH_ROW = limg.rows > fimg.rows ? limg.rows : fimg.rows;
    const int LOW_COL = limg.cols < fimg.cols ? limg.cols : fimg.cols;
    const int LOW_ROW = limg.rows < fimg.rows ? limg.rows : fimg.rows;
    
    gettimeofday(&start, NULL);
    for(int i = 0; i < HIGH_ROW; i++)
    {
        for(int j = 0; j < HIGH_COL; j++)
        {
            if(i > LOW_ROW || j > LOW_COL)
            {
                continue;
            }
            else
            {
                fimg.at<cv::Vec3b>(i, j) = 
                    0.75 * fimg.at<cv::Vec3b>(i, j) +
                    0.25 * limg.at<cv::Vec3b>(i, j);
            }
        }
    }
    gettimeofday(&end, NULL);
    printf("Sequential Result:\n\t"
           "-%ld.%ld seconds\n",
           end.tv_sec-start.tv_sec,
           end.tv_usec-start.tv_usec);

    cv::namedWindow("Sequential", WINDOW_AUTOSIZE);
    imshow("Sequential", fimg);
    waitKey(0);

    return 0;
}