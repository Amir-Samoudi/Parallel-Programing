# Parallel Programming Projects

This repository contains the projects for the **"Parallel Programming"** course.  
The assignments provide practical experience with different parallel programming paradigms, from low-level data parallelism to high-level GPU computing.

---

## Topics Covered
- **Data Level Parallelism (DLP):** Programming with Intel's SIMD (Single Instruction, Multiple Data) architecture.  
- **Thread Level Parallelism (TLP):** Multi-threaded programming using **OpenMP** and **POSIX threads**.  
- **GPU Programming:** Introduction to NVIDIA GPU architecture and parallel programming with **CUDA**.  
- **Performance Analysis:** Using tools like **Intel Parallel Studio** to analyze, debug, and optimize parallel code.  

---

## Projects

### Assignment 1: Data Level Parallelism with SIMD
- **Topic:** SIMD Programming  
- **Goal:** Accelerate image and video processing tasks using data-level parallelism.  
- **Description:**  
  - Leveraged Intel's SIMD architecture to process multiple data points simultaneously.  
  - Used SIMD intrinsics to implement optimized processing algorithms.  
  - Demonstrated the performance benefits of DLP in media-related applications.  

---

### Assignment 2: Thread Level Parallelism with OpenMP
- **Topic:** OpenMP  
- **Goal:** Parallelize complex simulations using the OpenMP framework.  
- **Description:**  
  - **Mandelbrot & Julia Sets:** Distributed fractal generation across multiple CPU threads.  
  - **Monte Carlo Pi Estimation:** Approximated π using a parallel Monte Carlo method.  
  - Showcased the ease and effectiveness of OpenMP for loop-heavy computations.  

---

### Assignment 3: Producer-Consumer Problem with POSIX Threads
- **Topic:** POSIX Threads (Pthreads)  
- **Goal:** Simulate a classic synchronization problem using the POSIX threads library.  
- **Description:**  
  - Implemented a bakery simulation of the **producer-consumer problem**.  
  - Used **mutexes** and **condition variables** to synchronize access and prevent race conditions.  

---

### Assignment 4: Performance Analysis with Intel Parallel Studio
- **Topic:** Code Optimization and Debugging  
- **Goal:** Apply a systematic methodology to analyze and improve parallel program performance.  
- **Description:** Followed a **4-step workflow** with Intel Parallel Studio:  
  1. **Analyze:** Profile the program to identify bottlenecks.  
  2. **Parallelize:** Add parallelism using OpenMP.  
  3. **Debug:** Fix concurrency issues (e.g., race conditions).  
  4. **Improve:** Optimize memory usage and overall performance.  

---

### Assignment 5: GPU Programming with CUDA
- **Topic:** CUDA Programming  
- **Goal:** Gain hands-on experience with GPU parallelism and compare against CPU-based methods.  
- **Description:**  
  - **Performance Comparison:** Implemented a parallel task in both CUDA and Python's multiprocessing.  
  - **Sphere Rendering:** Built a CUDA program to render a 3D sphere from multiple angles, highlighting the GPU’s strength in graphics and massively parallel tasks.  
