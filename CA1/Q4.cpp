// Amir Hosein Samoudi     Id: 810100108
// Armin Ghasemi           Id: 810100198

#include <opencv2/opencv.hpp>
#include <ipp.h>
#include <iostream>
#include <immintrin.h>

void serialMotionDetection(cv::VideoCapture& cap, cv::Mat& previous_frame, cv::Mat& motion_frame, cv::VideoWriter& output) {
    cv::Mat current_frame;
    int diff;

    while (cap.read(current_frame)) {
        for (int i = 0; i < current_frame.rows; i++) {
            uchar* prevRow = previous_frame.ptr<uchar>(i);
            uchar* currRow = current_frame.ptr<uchar>(i);
            uchar* motionRow = motion_frame.ptr<uchar>(i);

            for (int j = 0; j < current_frame.cols * 3; j++) {
                diff = currRow[j] - prevRow[j];
                motionRow[j] = static_cast<uchar>(std::abs(diff));
            }
        }
        output.write(motion_frame);
        previous_frame = current_frame.clone();
    }
}

void parallelMotionDetection(cv::VideoCapture& cap, cv::Mat& previous_frame, cv::Mat& motion_frame, cv::VideoWriter& output) {
    cv::Mat current_frame;
    __m128i prevPixels, currPixels, diff;
    uchar* prevRow,* currRow,* motionRow;

    while (cap.read(current_frame)) {
        for (int i = 0; i < current_frame.rows; i++) {
            prevRow = previous_frame.ptr<uchar>(i);
            currRow = current_frame.ptr<uchar>(i);
            motionRow = motion_frame.ptr<uchar>(i);

            for (int j = 0; j <= (current_frame.cols * 3) - 16; j += 16) {
                prevPixels = _mm_loadu_si128((__m128i*)(prevRow + j));
                currPixels = _mm_loadu_si128((__m128i*)(currRow + j));

                diff = _mm_subs_epu8(_mm_max_epu8(prevPixels, currPixels), _mm_min_epu8(prevPixels, currPixels));
                _mm_storeu_si128((__m128i*)(motionRow + j), diff);
            }
            /*for (; j < current_frame.cols * 3; j++) {
                int diff = currRow[j] - prevRow[j];
                motionRow[j] = static_cast<uchar>(std::abs(diff));
            }*/
        }
        output.write(motion_frame);
        previous_frame = current_frame.clone();
    }
}

int main() {
    cv::VideoCapture cap("Q4.mp4");
    if (!cap.isOpened()) {
        std::cerr << "Error opening video file!" << std::endl;
        return -1;
    }

    //video properties
    int frame_width = static_cast<int>(cap.get(cv::CAP_PROP_FRAME_WIDTH));
    int frame_height = static_cast<int>(cap.get(cv::CAP_PROP_FRAME_HEIGHT));
    int fps = static_cast<int>(cap.get(cv::CAP_PROP_FPS));

    // Define the codec
    cv::VideoWriter serialOutput("motion_detection_serial_output.mp4", cv::VideoWriter::fourcc('M', 'J', 'P', 'G'), fps, cv::Size(frame_width, frame_height));
    cv::VideoWriter parallelOutput("motion_detection_parallel_output.mp4", cv::VideoWriter::fourcc('M', 'J', 'P', 'G'), fps, cv::Size(frame_width, frame_height));

    if (!serialOutput.isOpened() || !parallelOutput.isOpened()) {
        std::cerr << "Error opening output video files!" << std::endl;
        return -1;
    }

    //first frame
    cv::Mat previous_frame, motion_frame;
    if (!cap.read(previous_frame)) {
        std::cerr << "Error reading the first frame!" << std::endl;
        return -1;
    }

    motion_frame = cv::Mat::zeros(previous_frame.size(), previous_frame.type());

    // 1. Serial Motion Detection
    cap.set(cv::CAP_PROP_POS_FRAMES, 1);
    Ipp64u startSerial = ippGetCpuClocks();
    serialMotionDetection(cap, previous_frame, motion_frame, serialOutput);
    Ipp64u endSerial = ippGetCpuClocks();
    std::cout << "Serial clock cycles: " << endSerial - startSerial << std::endl;

    // 2. Parallel Motion Detection
    cap.set(cv::CAP_PROP_POS_FRAMES, 0); // Reset video to start
    cap.read(previous_frame);

    Ipp64u startParallel = ippGetCpuClocks();
    parallelMotionDetection(cap, previous_frame, motion_frame, parallelOutput);
    Ipp64u endParallel = ippGetCpuClocks();
    std::cout << "Parallel clock cycles: " << endParallel - startParallel << std::endl;

    // Calculate speedup
    double speedup = static_cast<double>(endSerial - startSerial) / (endParallel - startParallel);
    std::cout << "Speedup: " << speedup << std::endl;

    // Cleanup
    cap.release();
    serialOutput.release();
    parallelOutput.release();

    std::cout << "Motion detection videos created successfully!" << std::endl;
    return 0;
}
