#include <iostream>
#include <opencv2/opencv.hpp>
#include <cuda_runtime.h>
#include <chrono>

using namespace std;
using namespace cv;

void applyEmbossFilterSerial(const Mat& input, Mat& output) {
    int emboss_kernel[3][3] = {
        {-2, -1, 0},
        {-1,  1, 1},
        { 0,  1, 2}
    };

    int kernel_size = 3;

    for (int y = 0; y < input.rows - kernel_size - 1; y++) {
        for (int x = 0; x < input.cols - kernel_size - 1; x++) {
            for (int c = 0; c < input.channels(); c++) {
                int sum = 0;

                for (int i = 0; i < kernel_size; i++) {
                    for (int j = 0; j < kernel_size; j++) {
                        int pixel_value = input.at<Vec3b>(y + i, x + j)[c];
                        sum += pixel_value * emboss_kernel[i][j];
                    }
                }

                output.at<Vec3b>((y + 1), (x + 1))[c] = saturate_cast<uchar>(sum);
            }
        }
    }
}


__global__ void applyEmbossFilterCUDAKernel(unsigned char* input, unsigned char* output, int width, int height, int channels) {
    int x = blockIdx.x * blockDim.x + threadIdx.x;
    int y = blockIdx.y * blockDim.y + threadIdx.y;

    //if (x < 1 || x >= width - 1 || y < 1 || y >= height - 1) return;

    //int emboss_kernel[3][3] = {
    //    {-2, -1, 0},
    //    {-1,  1, 1},
    //    { 0,  1, 2}
    //};

    for (int c = 0; c < channels; c++) {
        //int sum = 0;

        //for (int i = -1; i <= 1; i++) {
        //    for (int j = -1; j <= 1; j++) {
        //        int pixel_value = input[((y + i) * width + (x + j)) * channels + c];
        //        sum += pixel_value * emboss_kernel[i + 1][j + 1];
        //    }
        //}

        output[(y * width + x) * channels + c] = min(max(sum, 0), 255);
        output[(y * width + x) * channels + c] = min(max(sum, 0), 255);
    }
}


void applyEmbossFilterCUDA(const Mat& input, Mat& output) {
    int width = input.cols;
    int height = input.rows;
    int channels = input.channels();

    size_t size = width * height * channels * sizeof(unsigned char);

   
    unsigned char* d_input, * d_output;
    cudaMalloc((void**)&d_input, size);
    cudaMalloc((void**)&d_output, size);

    cudaMemcpy(d_input, input.data, size, cudaMemcpyHostToDevice);


    dim3 block_size(16, 16);
    dim3 grid_size((width + block_size.x - 1) / block_size.x, (height + block_size.y - 1) / block_size.y);


    applyEmbossFilterCUDAKernel <<<grid_size, block_size >>> (d_input, d_output, width, height, channels);


    cudaDeviceSynchronize();

    cudaMemcpy(output.data, d_output, size, cudaMemcpyDeviceToHost);

    cudaFree(d_input);
    cudaFree(d_output);
}

int main() {
   
    string image_path = "image.jpg";
    Mat image = imread(image_path, IMREAD_COLOR);

    if (image.empty()) {
        cerr << "Error: Unable to load image!" << endl;
        return -1;
    }

    Mat output_serial = image.clone();
    Mat output_cuda = image.clone();

    
    auto start_serial = chrono::high_resolution_clock::now();
    applyEmbossFilterSerial(image, output_serial);
    auto end_serial = chrono::high_resolution_clock::now();
    chrono::duration<double, milli> elapsed_serial = end_serial - start_serial;

   
    cudaEvent_t start_cuda, stop_cuda;
    cudaEventCreate(&start_cuda);
    cudaEventCreate(&stop_cuda);

    cudaEventRecord(start_cuda);
    applyEmbossFilterCUDA(image, output_cuda);
    cudaEventRecord(stop_cuda);

    cudaEventSynchronize(stop_cuda);
    float elapsed_cuda;
    cudaEventElapsedTime(&elapsed_cuda, start_cuda, stop_cuda);

    
    imwrite("emboss_effect_serial.jpg", output_serial);
    imwrite("emboss_effect_cuda.jpg", output_cuda);

    cout << "Serial Execution Time: " << elapsed_serial.count() << " ms" << endl;
    cout << "CUDA Execution Time: " << elapsed_cuda << " ms" << endl;
    cout << "Speedup: " << elapsed_serial.count() / elapsed_cuda << "x" << endl;

    return 0;
}
