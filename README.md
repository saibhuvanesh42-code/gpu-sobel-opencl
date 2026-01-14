# GPU Sobel Edge Detection with OpenCL

This project implements **Sobel edge detection with a grid overlay** using **OpenCL on the GPU** and compares its performance against an equivalent **CPU implementation**.

The application processes a real RGB image, detects edges using the Sobel operator, overlays a red grid pattern, and measures the execution time of both CPU and GPU versions to demonstrate parallel speedup.



## Motivation

Image processing algorithms such as Sobel edge detection operate independently on each pixel, making them ideal for **data-parallel execution**.  
This project demonstrates how GPU acceleration using OpenCL can significantly outperform a traditional CPU-based implementation.


## Features

- Sobel edge detection on RGB images  
- Internal grayscale conversion for edge detection  
- Red grid overlay (3 pixels wide every 20 pixels)  
- GPU implementation using OpenCL  
- CPU reference implementation  
- CPU vs GPU timing comparison using `std::chrono`  
- Tested on NVIDIA GTX 1650  

---

## Example Output

The output image contains:
- Red Sobel edges highlighting image boundaries  
- Red horizontal and vertical grid lines  
- Original color image preserved elsewhere  

All processing is performed on the GPU.

## Performance Results

Sample timing results on an NVIDIA GTX 1650:

CPU time: ~180 ms
GPU time: ~14 ms
Speedup: ~12×

## Project Structure

gpu-sobel-opencl/
├── test_opencl.cpp # CPU + GPU implementation and benchmarking
├── sobel_and_grid.cl # OpenCL kernel (Sobel + grid)
├── stb_image.h # Image loading
├── stb_image_write.h # Image saving
├── image.png # Input image
└── README.md


## How It Works

### Sobel Edge Detection
- RGB pixels are converted to grayscale using `(R + G + B) / 3`
- A 3×3 Sobel convolution computes horizontal and vertical gradients
- Edge magnitude is calculated as `|Gx| + |Gy|`
- Pixels above a threshold are colored red

### Grid Overlay
- A red grid is drawn using modulo operations on pixel coordinates
- Grid lines are 3 pixels wide and repeat every 20 pixels

### Parallel Execution
- Each GPU work-item processes one pixel
- CPU and GPU implementations use identical logic for fair benchmarking

---

## Build Instructions (Windows)

### Requirements
- Windows 10/11  
- NVIDIA GPU with OpenCL support  
- Visual Studio Build Tools (x64)  
- OpenCL headers and library (installed via vcpkg)  

### Compile

Open **x64 Native Tools Command Prompt for Visual Studio** and run:

```bat
cl test_opencl.cpp ^
  /I C:\Users\saibh\Desktop\vcpkg\installed\x64-windows\include ^
  /link /LIBPATH:C:\Users\saibh\Desktop\vcpkg\installed\x64-windows\lib OpenCL.lib
Run
bat
Copy code
test_opencl.exe
The program will:

Execute Sobel + grid on the CPU

Execute Sobel + grid on the GPU

Print CPU and GPU execution times

Save the output image as output.png

Key Learnings
Implemented data-parallel image processing using OpenCL

Benchmarked CPU vs GPU performance accurately

Learned GPU memory management and kernel execution

Structured a professional OpenCL project with external kernel files

