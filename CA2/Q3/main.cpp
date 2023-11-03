#include <opencv4/opencv2/highgui/highgui.hpp>
#include <opencv4/opencv2/imgproc/imgproc.hpp>
#include <opencv4/opencv2/core.hpp>
#include <stdio.h>
#include <x86intrin.h>
#include <sys/time.h>
#include <stdlib.h>

using namespace std;
using namespace cv;

#define FIRST_IMAGE "assets/1.png"
#define SECOND_IMAGE "assets/2.png"

int __diff_serial(Mat A, Mat B) {
    Mat D;
    struct timeval start, end;
    int NROWS = A.rows;
    int NCOLS = A.cols;

	D.create(NROWS, NCOLS, CV_8UC1);

    uint8_t* AD = (uint8_t*)A.data;
    uint8_t* BD = (uint8_t*)B.data;
    uint8_t* DD = (uint8_t*)D.data;

    gettimeofday(&start, NULL);
    for(int i = 0; i < NROWS; i++) {
        for(int j = 0; j < NCOLS; j++) {
            int p = i * NCOLS + j;
            DD[p] = abs(AD[p] - BD[p]);
        }
	}
    gettimeofday(&end, NULL);

	long seconds = (end.tv_sec - start.tv_sec);
	long int execution_time = ((seconds * 1000000) + end.tv_usec) - (start.tv_usec);
	printf("Serial Method:\n");
	printf("\tExecution time in microseconds: %ld\n\n", execution_time);
    imwrite("Q3 Serial.png", D);
	return execution_time;
}

int __diff_parallel(Mat A, Mat B) {
    Mat D;
    struct timeval start, end;
    int NROWS = A.rows;
    int NCOLS = A.cols;
    D.create(NROWS, NCOLS, CV_8UC1);

    __m128i* AD = (__m128i*) A.data;
    __m128i* BD = (__m128i*) B.data;
    __m128i* DD = (__m128i*) D.data;

    __m128i a, b, sub0, sub1, abs;

    gettimeofday(&start, NULL);
    for(int i = 0; i < NROWS; i++) {
        for(int j = 0; j < NCOLS/16; j++) {
            int p = i * (NCOLS/16) + j;
            a = _mm_loadu_si128((__m128i*)(AD + p));
            b = _mm_loadu_si128((__m128i*)(BD + p));
            sub0 = _mm_subs_epu8(a, b);
            sub1 = _mm_subs_epu8(b, a);
            abs = _mm_or_si128(sub0, sub1);
            _mm_store_si128((__m128i*)(DD + p), abs);
        }
	}
    gettimeofday(&end, NULL);

	long seconds = (end.tv_sec - start.tv_sec);
	long int execution_time = ((seconds * 1000000) + end.tv_usec) - (start.tv_usec);
	printf("Parallel Method:\n");
	printf("\tExecution time in microseconds: %ld\n\n", execution_time);
    imwrite("Q3 Parallel.png", D);
	return execution_time;
}

int main() {
        // Show group members
    printf("Group Members:\n");
    printf("\t- Ali Ghanbari [810199473]\n");
    printf("\t- Behrad Elmi  [810199557]\n\n");

    Mat A = imread(FIRST_IMAGE, IMREAD_GRAYSCALE);
    Mat B = imread(SECOND_IMAGE, IMREAD_GRAYSCALE);
	if (A.empty() || B.empty()) {
        printf("Error reading images.");
        return -1;
    }

   	long int ST = __diff_serial(A, B);
	long int PT = __diff_parallel(A, B);
	printf("Speed up: %.2f\n", ((float)ST/(float)PT));
  	return 0;
}
