import cv2
import numpy as np
import time
from multiprocessing import Pool
import matplotlib.pyplot as plt

emboss_kernel = np.array([[-2, -1, 0],
                          [-1,  1, 1],
                          [ 0,  1, 2]])



def process_block(args):
    image, kernel, start_row, end_row = args
    kernel_size = kernel.shape[0]
    pad = kernel_size // 2
    h, w, c = image.shape

    output_block = np.zeros((end_row - start_row - 2 * pad, w - 2 * pad, c), dtype=np.uint8)

    for i in range(start_row + pad, end_row - pad):
        for j in range(pad, w - pad):
            for channel in range(c):
                region = image[i - pad:i + pad + 1, j - pad:j + pad + 1, channel]
                output_block[i - start_row - pad, j - pad, channel] = np.clip(np.sum(region * kernel), 0, 255)
    
    return output_block

if __name__ == "__main__":
    image = cv2.imread("image.jpg")

    kernel_size = emboss_kernel.shape[0]
    pad = kernel_size // 2

    h, w, c = image.shape

    start_time_sequential = time.perf_counter()

    output_image_sequential = np.zeros((h - 2 * pad, w - 2 * pad, c), dtype=np.uint8)

    for i in range(pad, h - pad):
        for j in range(pad, w - pad):
            for channel in range(c):
                region = image[i - pad:i + pad + 1, j - pad:j + pad + 1, channel]
                output_image_sequential[i - pad, j - pad, channel] = np.clip(np.sum(region * emboss_kernel), 0, 255)

    end_time_sequential = time.perf_counter()

    sequential_time = end_time_sequential - start_time_sequential

    cv2.imwrite("emboss_effect_sequential.jpg", output_image_sequential)

    num_processes_list = list(range(1, 9))
    speedup_list = []
    efficiency_list = []

    for num_processes in num_processes_list:
        block_height = (h - 2 * pad) // num_processes
        boundaries = [(i * block_height + pad, (i + 1) * block_height + pad) for i in range(num_processes)]
        boundaries[-1] = (boundaries[-1][0], h - pad)  # Adjust the last block to include remaining rows

        args = [(image, emboss_kernel, start, end) for start, end in boundaries]

        start_time_parallel = time.perf_counter()

        with Pool(processes=num_processes) as pool:
            processed_blocks = pool.map(process_block, args)

        end_time_parallel = time.perf_counter()

        parallel_time = end_time_parallel - start_time_parallel

        speedup = sequential_time / parallel_time
        efficiency = speedup / num_processes

        speedup_list.append(speedup)
        efficiency_list.append(efficiency)

        output_image_parallel = np.vstack(processed_blocks)

        output_filename = f"emboss_effect_parallel_{num_processes}_processes.jpg"
        cv2.imwrite(output_filename, output_image_parallel)

    plt.figure(figsize=(12, 6))

    plt.subplot(1, 2, 1)
    plt.plot(num_processes_list, speedup_list, marker='o', label='Speedup')
    plt.title("Speedup vs Number of Processes")
    plt.xlabel("Number of Processes")
    plt.ylabel("Speedup")
    plt.grid()
    plt.legend()

    plt.subplot(1, 2, 2)
    plt.plot(num_processes_list, efficiency_list, marker='o', color='orange', label='Efficiency')
    plt.title("Efficiency vs Number of Processes")
    plt.xlabel("Number of Processes")
    plt.ylabel("Efficiency")
    plt.grid()
    plt.legend()

    plt.tight_layout()
    plt.savefig("performance_analysis.png")
    plt.show()
