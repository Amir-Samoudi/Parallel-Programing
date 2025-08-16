// Amir Hosein Samoudi     Id: 810100108
// Armin Ghasemi           Id: 810100198

#include <iostream>
#include <complex>
#include <opencv2/opencv.hpp>
#include <omp.h>
#include <ipp.h>

void MandelbrotSetSerial(int width, int height, int maxIterations, const std::string& filename, double initialC) {
    double realMin = -2.0 * initialC/4, realMax = 1.0 * initialC/4;
    double imagMin = -1.5 * initialC/4, imagMax = 1.5 * initialC/4;
    cv::Mat image(height, width, CV_8UC3);

    double real, imag;
    std::complex<double> c, z;
    int iteration;
    int r, g, b;

    for (int y = 0; y < height; ++y) {
        imag = imagMin + (y / static_cast<double>(height)) * (imagMax - imagMin);
        for (int x = 0; x < width; ++x) {
            real = realMin + (x / static_cast<double>(width)) * (realMax - realMin);
            c = std::complex<double>(real, imag);
            z = 0;

            iteration = 0;
            for (iteration = 0; iteration < maxIterations; ++iteration) {
                z = z * z + c;
                if (std::abs(z) > 2.0) {
                    break;
                }
            }

            if (iteration == maxIterations) {
                image.at<cv::Vec3b>(y, x) = cv::Vec3b(0, 0, 0);
            }
            else {
                r = static_cast<int>(255 * (iteration / static_cast<double>(maxIterations)));
                g = static_cast<int>(128 * (iteration / static_cast<double>(maxIterations)));
                b = 255 - r;
                image.at<cv::Vec3b>(y, x) = cv::Vec3b(b, g, r);
            }
        }
    }
    cv::imwrite(filename, image);
    std::cout << "Mandelbrot Set image saved done. " << std::endl;
}

void MandelbrotSetParallel(int width, int height, int maxIterations, const std::string& filename, double initialC) {
    double realMin = -2.0 * initialC/4, realMax = 1.0 * initialC/4;
    double imagMin = -1.5 * initialC/4, imagMax = 1.5 * initialC/4;
    cv::Mat image(height, width, CV_8UC3);

    double real, imag;
    std::complex<double> c, z;
    int iteration;
    int r, g, b;

    #pragma omp parallel for schedule(dynamic) num_threads(6) private(real, imag, c, z, iteration, r, g, b) shared(image, realMin, realMax, imagMin, imagMax, width, height, maxIterations)
        for (int y = 0; y < height; ++y) {
            /*if (y == 0) {
                std::cout << "Num Threads: " << omp_get_num_threads() << std::endl;
            }*/
            imag = imagMin + (y / static_cast<double>(height)) * (imagMax - imagMin);
            for (int x = 0; x < width; ++x) {
                real = realMin + (x / static_cast<double>(width)) * (realMax - realMin);
                c = std::complex<double>(real, imag);
                z = 0;

                iteration = 0;
                for (iteration = 0; iteration < maxIterations; ++iteration) {
                    z = z * z + c;
                    if (std::abs(z) > 2.0) {
                        break;
                    }
                }

                if (iteration == maxIterations) {
                    r = g = b = 0;
                }
                else {
                    r = static_cast<int>(255 * (iteration / static_cast<double>(maxIterations)));
                    g = static_cast<int>(128 * (iteration / static_cast<double>(maxIterations)));
                    b = 255 - r;
                }

                image.at<cv::Vec3b>(y, x) = cv::Vec3b(b, g, r);
            }
        }

    cv::imwrite(filename, image);
    std::cout << "Mandelbrot Set parallel done. " << std::endl;
}

int main() {
    int width = 600;
    int height = 600;
    int maxIterations = 1000;
    std::string filenameSerial = "mandelbrot_set_serial.png";
    std::string filenameParallel = "mandelbrot_set_parallel.png";
    double initialC = 8.0;

    Ipp64u startSerial = ippGetCpuClocks();
    MandelbrotSetSerial(width, height, maxIterations, filenameSerial, initialC);
    Ipp64u endSerial = ippGetCpuClocks();
    std::cout << "Serial clock cycles: " << endSerial - startSerial << std::endl;

    Ipp64u startParallel = ippGetCpuClocks();
    MandelbrotSetParallel(width, height, maxIterations, filenameParallel, initialC);
    Ipp64u endParallel = ippGetCpuClocks();
    std::cout << "Parallel clock cycles: " << endParallel - startParallel << std::endl;

    double speedup = static_cast<double>(endSerial - startSerial) / (endParallel - startParallel);
    std::cout << "Speedup: " << speedup << std::endl;

    return 0;
}

