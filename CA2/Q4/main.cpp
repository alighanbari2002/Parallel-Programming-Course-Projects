#include <opencv4/opencv2/highgui/highgui.hpp>
#include <opencv4/opencv2/imgproc/imgproc.hpp>
#include <opencv4/opencv2/core.hpp>
#include <stdio.h>
#include <x86intrin.h>
#include <sys/time.h>
#include <stdlib.h>

using namespace std;
using namespace cv;

#define FIRST_IMAGE "./assets/front.png"
#define SECOND_IMAGE "./assets/logo.png"

int __attach_serial(Mat Img1, Mat Img2, Mat Result) {
    struct timeval start, end;
    
    int NCOLS = Img1.cols;
    int NROWS_2 = Img2.rows;
    int NCOLS_2 = Img2.cols;
    
    uint8_t* D1 = (uint8_t*)Img1.data;
    uint8_t* D2 = (uint8_t*)Img2.data;
    uint8_t* R = (uint8_t*)Result.data;

    gettimeofday(&start, NULL);
    for(int i = 0; i < NROWS_2; i++) {
        for(int j = 0; j < NCOLS_2; j++) {
            int p1 = i * NCOLS + j;
            int p2 = i * NCOLS_2 + j;
            R[p1] = (D1[p1] + D2[p2]) >> 2;
        }
    }
    gettimeofday(&end, NULL);

    long seconds = (end.tv_sec - start.tv_sec);
	long int execution_time = ((seconds * 1000000) + end.tv_usec) - (start.tv_usec);
    printf("Serial Method:\n");
	printf("\tExecution time in microseconds: %ld\n\n", execution_time);
    imwrite("Q2 Serial.png", Result);
    return execution_time;
}

int __attach_parallel(Mat Img1, Mat Img2, Mat Result) {
    struct timeval start, end;

    int NCOLS = Img1.cols;
    int NROWS_2 = Img2.rows;
    int NCOLS_2 = Img2.cols;

    __m128i* D1 = (__m128i*) Img1.data;
    __m128i* D2 = (__m128i*) Img2.data;
    __m128i* R = (__m128i*) Result.data;

    __m128i N = _mm_set1_epi8(63);
	__m128i im1, im2, res;
    gettimeofday(&start, NULL);
    for (int i = 0; i < NROWS_2; i++) {
        for (int j = 0; j < NCOLS_2 / 16; j++) {
            int p1 = i * (NCOLS / 16) + j;
            int p2 = i * (NCOLS_2 / 16) + j;

            im1 = _mm_loadu_si128(D1 + p1);
            im2 = _mm_loadu_si128(D2 + p2);
            im2 = im2 >> 2;
            im2 = _mm_and_si128(N, im2);
            res = _mm_adds_epu8(im1, im2);
            _mm_store_si128(R + p1, res);
        }
    }
    gettimeofday(&end, NULL);

    long seconds = (end.tv_sec - start.tv_sec);
	long int execution_time = ((seconds * 1000000) + end.tv_usec) - (start.tv_usec);
    printf("Parallel Method:\n");
	printf("\tExecution time in microseconds: %ld\n\n", execution_time);
    imwrite("Q2 Parallel.png", Result);
	return execution_time;

}

int main() {
        // Show group members
    printf("Group Members:\n");
    printf("\t- Ali Ghanbari [810199473]\n");
    printf("\t- Behrad Elmi  [810199557]\n\n");

    Mat Img1 = imread(FIRST_IMAGE, IMREAD_GRAYSCALE);
    Mat Img2 = imread(SECOND_IMAGE, IMREAD_GRAYSCALE);
    Mat Result = imread(FIRST_IMAGE, IMREAD_GRAYSCALE);

    if (Img1.empty() || Img2.empty()) {
        printf("Error reading images.");
        return -1;
    }

	long int ST = __attach_serial(Img1, Img2, Result);
	long int PT = __attach_parallel(Img1, Img2, Result);
	printf("Speed up: %.2f\n", ((float)ST/(float)PT));

    return 0;
}