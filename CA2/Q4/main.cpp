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
    
    
    return 0;
}