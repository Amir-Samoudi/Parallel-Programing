// Amir Hosein Samoudi     Id: 810100108
// Armin Ghasemi           Id: 810100198

#include <iostream>
#include <complex>
#include <opencv2/opencv.hpp>
#include <omp.h>
#include <ipp.h>

void JuliaSetSerial(int width, int height, int maxIterations, const std::string& filename, std::complex<double> c) {
    double realMin = -2.0, realMax = 2.0;
    double imagMin = -2.0, imagMax = 2.0;
    cv::Mat image(height, width, CV_8UC3);

    double real, imag;
    std::complex<double> z;
    int iteration;
    int r, g, b;

    for (int y = 0; y < height; ++y) {
        imag = imagMin + (y / static_cast<double>(height)) * (imagMax - imagMin);
        for (int x = 0; x < width; ++x) {
            real = realMin + (x / static_cast<double>(width)) * (realMax - realMin);
            z = std::complex<double>(real, imag);

            iteration = 0;
            for (iteration = 0; iteration < maxIterations; ++iteration) {
                z = z * z + c;
                if (std::abs(z) > 2.0) {
                    break;
                }
            }
            if (iteration == maxIterations) {
                image.at<cv::Vec3b>(y, x) = cv::Vec3b(0, 0, 0); // Black
            }
            else {
                r = static_cast<int>(255 * (iteration / static_cast<double>(maxIterations)));
                g = static_cast<int>(128 * (iteration / static_cast<double>(maxIterations)));
                b = 255 - r;
                image.at<cv::Vec3b>(y, x) = cv::Vec3b(b, g, r); // Blue
            }
        }
    }

    cv::imwrite(filename, image);
    std::cout << "Julia Set image done. " << std::endl;
}

void JuliaSetParallel(int width, int height, int maxIterations, const std::string& filename, std::complex<double> c) {
    double realMin = -2.0, realMax = 2.0;
    double imagMin = -2.0, imagMax = 2.0;
    cv::Mat image(height, width, CV_8UC3);

    double real, imag;
    std::complex<double> z;
    int iteration;
    int r, g, b;

#pragma omp parallel for schedule(dynamic) num_threads(6) private(real, imag, z, iteration, r, g, b) shared(image, realMin, realMax, imagMin, imagMax, width, height, maxIterations, c)
    for (int y = 0; y < height; ++y) {
        imag = imagMin + (y / static_cast<double>(height)) * (imagMax - imagMin);
        for (int x = 0; x < width; ++x) {
            real = realMin + (x / static_cast<double>(width)) * (realMax - realMin);
            z = std::complex<double>(real, imag);

            iteration = 0;
            for (iteration = 0; iteration < maxIterations; ++iteration) {
                z = z * z + c;
                if (std::abs(z) > 2.0) {
                    break;
                }
            }

            if (iteration == maxIterations) {
                image.at<cv::Vec3b>(y, x) = cv::Vec3b(0, 0, 0); // Black
            }
            else {
                r = static_cast<int>(255 * (iteration / static_cast<double>(maxIterations)));
                g = static_cast<int>(128 * (iteration / static_cast<double>(maxIterations)));
                b = 255 - r;
                image.at<cv::Vec3b>(y, x) = cv::Vec3b(b, g, r); // Blue to Red
            }
        }
    }

    cv::imwrite(filename, image);
    std::cout << "Julia Set parallel image done. " << std::endl;
}

int main() {
    int width = 600;
    int height = 600;
    int maxIterations = 1000;
    //std::complex<double> c(-0.5, 0.5);
    std::complex<double> c(0.355, 0.355);
    std::string filenameSerial = "julia_set_serial.png";
    std::string filenameParallel = "julia_set_parallel.png";

    Ipp64u startSerial = ippGetCpuClocks();
    JuliaSetSerial(width, height, maxIterations, filenameSerial, c);
    Ipp64u endSerial = ippGetCpuClocks();
    std::cout << "Serial clock cycles: " << endSerial - startSerial << std::endl;

    Ipp64u startParallel = ippGetCpuClocks();
    JuliaSetParallel(width, height, maxIterations, filenameParallel, c);
    Ipp64u endParallel = ippGetCpuClocks();
    std::cout << "Parallel clock cycles: " << endParallel - startParallel << std::endl;

    double speedup = static_cast<double>(endSerial - startSerial) / (endParallel - startParallel);
    std::cout << "Speedup: " << speedup << std::endl;

    return 0;
}
