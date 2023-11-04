#include <string>
#include <vector>
#include <sys/time.h>
#include <cstdlib>
#include <x86intrin.h>
#include <tmmintrin.h>
#include <ctime>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>

#define M128_GRAY_INTERVAL 16
#define MIL 1000000
#define OUTDIR "../out/"

using namespace cv;

int main()
{
    struct timeval start, end;
    std::string first_frame = "../assets/1.png";
    std::string second_frame = "../assets/2.png";
    clock_t sserial, eserial, sparallel, eparallel;

    // Load frames
    Mat img1 = imread(first_frame, IMREAD_GRAYSCALE);
    Mat img2 = imread(second_frame, IMREAD_GRAYSCALE);
    if (img1.size() != img2.size())
    {
        printf("Illegal frames\n");
        exit(EXIT_FAILURE);
    }
    const unsigned NCOLS = img1.cols;
    const unsigned NROWS = img2.rows;

    // Serial
    cv::Mat seq_res(NROWS, NCOLS, CV_8U);

    // Define pointers
    uchar* seq_res_ptr = seq_res.data;
    uchar* img1_ptr = img1.data;
    uchar* img2_ptr = img2.data;


    gettimeofday(&start, NULL);
    sserial = clock();
    for(int i = 0; i < NROWS; i++)
    {
        for(int j = 0; j < NCOLS; j++)
        {
            *(seq_res_ptr + i * NCOLS + j) = abs(
                *(img1_ptr + i * NCOLS + j) - *(img2_ptr + i * NCOLS + j)
                );
        }
    }
    eserial = clock();
    gettimeofday(&end, NULL);

    img1.release();
    img2.release();
    img1_ptr = NULL;
    img2_ptr = NULL;

    printf("Serial Result:\n\t"
       "- %0.6lf seconds\n",
       (double)(MIL*(end.tv_sec - start.tv_sec) +
       end.tv_usec - start.tv_usec) / MIL);
    cv::namedWindow("Serial", cv::WINDOW_AUTOSIZE);
    imshow("Serial", seq_res);
    imwrite(OUTDIR "Q3 Serial.png", seq_res);
    
    // Parallel
    Mat pimg1 = imread(first_frame, IMREAD_GRAYSCALE);
    Mat pimg2 = imread(second_frame, IMREAD_GRAYSCALE);
    __m128i p1, p2, diff12, diff21, diff;

    gettimeofday(&start, NULL);
    sparallel = clock();
    for (int i = 0; i < NROWS; i++)
    {
        for (int j = 0; j < NCOLS; j += M128_GRAY_INTERVAL)
        {
            p1 = _mm_load_si128(reinterpret_cast<__m128i*>(pimg1.data + i*NCOLS + j));
            p2 = _mm_load_si128(reinterpret_cast<__m128i*>(pimg2.data + i*NCOLS + j));
            diff12 = _mm_subs_epu8(p1, p2);
            diff21 = _mm_subs_epu8(p2, p1);
            diff = _mm_or_si128(diff12, diff21); // Safe because one is zero see http://0x80.pl/notesen/2018-03-11-sse-abs-unsigned.html
            _mm_store_si128(reinterpret_cast<__m128i*>(pimg1.data + i*NCOLS + j), diff);
        }
    }
    eparallel = clock();
    gettimeofday(&end, NULL);
    printf("Parallel Result:\n\t"
           "- %0.6lf seconds\n",
           (double)(MIL*(end.tv_sec - start.tv_sec) +
           end.tv_usec - start.tv_usec) / MIL);
    cv::namedWindow("Parallel", cv::WINDOW_AUTOSIZE);
    imshow("Parallel", pimg1);
    imwrite(OUTDIR "Q3 Parallel.png", pimg1);
    
    printf("Speedup:\n\t"
       "- %0.4f\n", (float)(eserial-sserial) / (float)((eparallel-sparallel)));
    waitKey(0);

    return 0;
}