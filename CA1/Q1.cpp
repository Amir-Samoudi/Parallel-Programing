// Amir Hosein Samoudi     Id: 810100108
// Armin Ghasemi           Id: 810100198

#include <opencv2/opencv.hpp>
#include <ipp.h>
#include <immintrin.h>
#include <iostream>

using namespace cv;

// Serial blending function
void blendSerial( Mat& front, Mat& logo, Mat& output) {
    const int logoRows = logo.rows;
    const int logoCols = logo.cols;

    uchar* frontRow;
    uchar* logoRow;
    int blendedLogo;

    for (int i = 0; i < logoRows; i++) {
        frontRow = output.ptr<uchar>(i);
        logoRow = logo.ptr<uchar>(i);

        for (int j = 0; j < logoCols * 3; j++) {
            // Approximate blend factor by shifting
            blendedLogo = (logoRow[j] >> 1) + (logoRow[j] >> 3);
            frontRow[j] = cv::saturate_cast<uchar>(frontRow[j] + blendedLogo);
        }
    }
}

// Parallel SIMD blending function
void blendParallel( Mat& front, Mat& logo, Mat& output) {
    const int logoRows = logo.rows;
    const int logoCols = logo.cols;

    __m128i frontPixels, logoPixels, result;
    __m128i logo_shift_1, logo_shift_3;
    __m128i MSB1Zero = _mm_set1_epi8(0x7F);
    __m128i MSB3Zero = _mm_set1_epi8(0x1F);

    uchar* frontRow;
    uchar* logoRow;

    for (int i = 0; i < logoRows; i++) {
        frontRow = output.ptr<uchar>(i);
        logoRow = logo.ptr<uchar>(i);

        for (int j = 0; j <= (logoCols * 3) - 16; j += 16) {
            // Load 16 consecutive bytes
            frontPixels = _mm_loadu_si128((__m128i*)(frontRow + j));
            logoPixels = _mm_loadu_si128((__m128i*)(logoRow + j));

            // Approximate blend factor by shifting
            logo_shift_1 = _mm_srli_epi16(logoPixels, 1);
            logo_shift_1 = _mm_and_si128(MSB1Zero, logo_shift_1);

            logo_shift_3 = _mm_srli_epi16(logoPixels, 3);
            logo_shift_3 = _mm_and_si128(MSB3Zero, logo_shift_3);

            result = _mm_adds_epu8(frontPixels, _mm_adds_epu8(logo_shift_1, logo_shift_3));

            // Store result
            _mm_storeu_si128((__m128i*)(frontRow + j), result);
        }
    }
}

int main() {
    // Load images
    Mat front = imread("front.png", IMREAD_COLOR);
    Mat logo = imread("logo.png", IMREAD_COLOR);

    if (front.empty() || logo.empty()) {
        std::cerr << "Error loading images!" << std::endl;
        return -1;
    }

    if (logo.rows > front.rows || logo.cols > front.cols) {
        std::cerr << "Logo image is larger than the front image!" << std::endl;
        return -1;
    }

    Mat serialOutput = front.clone();
    Mat parallelOutput = front.clone();

    // Measure and execute serial blending
    Ipp64u startSerial = ippGetCpuClocks();
    blendSerial(front, logo, serialOutput);
    Ipp64u endSerial = ippGetCpuClocks();
    std::cout << "Serial clock cycles: " << endSerial - startSerial << std::endl;

    // Measure and execute parallel blending
    Ipp64u startParallel = ippGetCpuClocks();
    blendParallel(front, logo, parallelOutput);
    Ipp64u endParallel = ippGetCpuClocks();
    std::cout << "Parallel clock cycles: " << endParallel - startParallel << std::endl;

    // Calculate speedup
    double speedup = static_cast<double>(endSerial - startSerial) / (endParallel - startParallel);
    std::cout << "Speedup: " << speedup << std::endl;

    // Save images
    imwrite("serial_output.png", serialOutput);
    imwrite("parallel_output.png", parallelOutput);

    // Display 
    imshow("Serial Blended Image", serialOutput);
    imshow("Parallel Blended Image", parallelOutput);
    //imshow("Difference of two Images", parallelOutput - serialOutput);
    waitKey(0);

    return 0;
}
