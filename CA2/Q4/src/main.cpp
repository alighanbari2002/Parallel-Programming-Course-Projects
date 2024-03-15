#include <stdio.h>
#include <stdlib.h>
#include <chrono>
#include <sstream>
#include <cassert>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#ifdef 		_WIN32
#include <intrin.h>
#else
#include <x86intrin.h>
#endif

using std::chrono::high_resolution_clock;
using std::chrono::duration_cast;
using std::chrono::nanoseconds;
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

    // Start the timer
	auto start = high_resolution_clock::now();

    for(int row = 0; row < out_img_serial.rows; ++row)
    {
        const uchar* front_row = front.ptr<uchar>(row);
        const uchar* logo_row = logo.ptr<uchar>(row);
        uchar* out_img_row = out_img_serial.ptr<uchar>(row);

        for(int col = 0; col < out_img_serial.cols; ++col)
        {
            if(row < logo.rows && col < logo.cols)
            {
                int new_pixel = front_row[col] + alpha * logo_row[col];
                out_img_row[col] = new_pixel > 255 ? 255 : static_cast<uchar>(new_pixel);
            }
        }
    }

    // Stop the timer
	auto finish = high_resolution_clock::now();

    imwrite(OUTPUT_DIR "serial output.png", out_img_serial);
    out_img_serial.release();

	ll execution_time = duration_cast<nanoseconds>(finish - start).count();

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
    
    __m128i_u logo_chunk, front_chunk;
    __m128i_u* output_chunk;

    // Divide by 4 
    // 00 --> xxxx xxxx xxxx xx --> xx
    // 00xx xxxx xxxx xxxx --> 00xx xxxx 00xx xxxx
    const __m128i_u mask_divide_by_4 = _mm_set1_epi16(0xFF3F);

    const int logo_width = logo.cols - M128_GRAY_INTERVAL;

    // Start the timer
	auto start = high_resolution_clock::now();

    for(int row = 0; row < out_img_parallel.rows; ++row)
    {
        const uchar* front_row = front.ptr<uchar>(row);
        const uchar* logo_row = logo.ptr<uchar>(row);
        uchar* out_img_row = out_img_parallel.ptr<uchar>(row);

        for(int col = 0; col < out_img_parallel.cols; col += M128_GRAY_INTERVAL)
        {
            front_chunk = _mm_lddqu_si128(reinterpret_cast<const __m128i_u*>(front_row + col));
            logo_chunk  = _mm_lddqu_si128(reinterpret_cast<const __m128i_u*>(logo_row + col));
            output_chunk = reinterpret_cast<__m128i_u*>(out_img_row + col);

            if(row < logo.rows && col < logo_width)
            {
                // Apply alpha blending to logo chunk
                logo_chunk = _mm_srli_epi16(logo_chunk, 2);
                logo_chunk = _mm_and_si128(logo_chunk, mask_divide_by_4);

                // Store blended result
                _mm_storeu_si128(output_chunk, _mm_adds_epu8(logo_chunk, front_chunk));
            }
        }
    }

    // Stop the timer
	auto finish = high_resolution_clock::now();

    imwrite(OUTPUT_DIR "parallel output.png", out_img_parallel);
    out_img_parallel.release();

	ll execution_time = duration_cast<nanoseconds>(finish - start).count();

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
