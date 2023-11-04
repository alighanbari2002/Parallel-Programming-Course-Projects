#include <string>
#include <vector>
#include <sys/time.h>
#include <cstdlib>
#include <x86intrin.h>
#include <tmmintrin.h>
#include <emmintrin.h>
#include <ctime>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>

#define M128_GRAY_INTERVAL 16
#define MIL 1000000

using namespace cv;

int main()
{
    // Show Group Members
    printf("Group Members:\n");
    printf("\t- Ali Ghanbari [810199473]\n");
    printf("\t- Behrad Elmi  [810199557]\n\n");
    struct timeval start, end;
    std::string logo = "./assets/logo.png";
    std::string front = "./assets/front.png";
    clock_t sserial, eserial, sparallel, eparallel;

    Mat limg = imread(logo, IMREAD_GRAYSCALE);
    Mat fimg = imread(front, IMREAD_GRAYSCALE);
    uchar* l_ptr; 
    uchar* f_ptr; 
    const int HIGH_COL = limg.cols > fimg.cols ? limg.cols : fimg.cols;
    const int HIGH_ROW = limg.rows > fimg.rows ? limg.rows : fimg.rows;
    const int LOW_COL = limg.cols < fimg.cols ? limg.cols : fimg.cols;
    const int LOW_ROW = limg.rows < fimg.rows ? limg.rows : fimg.rows;
    

    // Sequential
    gettimeofday(&start, NULL);
    sserial = clock();
    for(int i = 0; i < HIGH_ROW; i++)
    {
        l_ptr = limg.ptr<uchar>(i);
        f_ptr = fimg.ptr<uchar>(i);
        for(int j = 0; j < HIGH_COL; j++)
        {
            if(i > LOW_ROW || j > LOW_COL)
            {
                continue;
            }
            else
            {
                if((int)(3*f_ptr[j]/4 + l_ptr[j]/4) > 255)
                {
                    f_ptr[j] = 255;
                }
                else
                {
                    f_ptr[j] = 3*f_ptr[j]/4 + l_ptr[j]/4; 
                }
            }
        }
    }
    eserial = clock();
    gettimeofday(&end, NULL);
    printf("Serial Result:\n\t"
       "- %0.6lf seconds\n",
       (double)(MIL*(end.tv_sec - start.tv_sec) +
       end.tv_usec - start.tv_usec) / MIL);

    cv::namedWindow("Sequential", WINDOW_AUTOSIZE);
    imshow("Sequential", fimg);
    imwrite("Q4 Serial.png", fimg);
    
    // Free allocated memory and matrices
    limg.release();
    fimg.release();

    // Parallel
    Mat plimg = imread(logo, IMREAD_GRAYSCALE);
    Mat pfimg = imread(front, IMREAD_GRAYSCALE);

    __m128i_u pf, pl, pftemp;
    
    gettimeofday(&start, NULL);
    sparallel = clock();
    for(int i = 0; i < HIGH_ROW; i++)
    {
        for(int j = 0; j < HIGH_COL; j+= 16)
        {
            if(i > LOW_ROW || j > LOW_COL-16)
            {
                continue;
            }
            else
            {
                pf = _mm_loadu_si128(reinterpret_cast<__m128i*>(
                    pfimg.data + (i * pfimg.cols) + j));
                pl = _mm_loadu_si128(reinterpret_cast<__m128i*>(
                    plimg.data + (i * plimg.cols) + j));
                
                // Divide by 4 
                // 00 --> xxxx xxxx xxxx xx --> xx
                // 00xx xxxx xxxx xxxx --> 00xx xxxx 00xx xxxx
                pl = _mm_srli_epi16(pl, 2);
                pf = _mm_srli_epi16(pf, 2);
                pl = _mm_and_si128(pl, _mm_set1_epi16(0xFF3F));
                pf = _mm_and_si128(pf, _mm_set1_epi16(0xFF3F));
                pftemp = pf;
                
                // Multiply front by 3 achieving 0.75
                // x <-- xxx xxxx xxxx xxxx <--0
                // xxxx xxxx xxxx xxx0
                // xxxx xxx0 xxxx xxx0
                pf = _mm_slli_epi16(pf, 1);
                pf = _mm_and_si128(pf, _mm_set1_epi16(0xFEFF));
                pf = _mm_add_epi8(pf, pftemp);

                _mm_storeu_si128(reinterpret_cast<__m128i*>(
                    pfimg.data + (i * pfimg.cols) + j), _mm_add_epi8(pl, pf));
            }
        }
    }
    eparallel = clock();
    gettimeofday(&end, NULL);
    cv::namedWindow("Parallel", WINDOW_AUTOSIZE);
    imshow("Parallel", pfimg);
    imwrite("Q4 Parallel.png", pfimg);
    printf("Parallel Result:\n\t"
           "- %0.6lf seconds\n",
           (double)(MIL*(end.tv_sec - start.tv_sec) +
           end.tv_usec - start.tv_usec) / MIL);
    printf("Speedup:\n\t"
           "- %0.4f\n", (float)(eserial-sserial) / (float)((eparallel-sparallel)));
    waitKey(0);

    return 0;
}