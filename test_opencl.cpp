#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "stb_image.h"
#include "stb_image_write.h"

#include <CL/cl.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>

// ---------------- CPU VERSION ----------------
void sobel_and_grid_cpu(unsigned char *img, int width, int height)
{
    std::vector<unsigned char> copy(img, img + width * height * 3);

    for (int y = 1; y < height - 1; y++)
    {
        for (int x = 1; x < width - 1; x++)
        {

            int idx = (y * width + x) * 3;

            int gray[9];
            int k = 0;

            for (int j = -1; j <= 1; j++)
            {
                for (int i = -1; i <= 1; i++)
                {
                    int nidx = ((y + j) * width + (x + i)) * 3;
                    gray[k++] =
                        (copy[nidx] + copy[nidx + 1] + copy[nidx + 2]) / 3;
                }
            }

            int gx =
                -gray[0] + gray[2] - 2 * gray[3] + 2 * gray[5] - gray[6] + gray[8];

            int gy =
                -gray[0] - 2 * gray[1] - gray[2] + gray[6] + 2 * gray[7] + gray[8];

            int magnitude = abs(gx) + abs(gy);

            if (magnitude > 150)
            {
                img[idx + 0] = 255;
                img[idx + 1] = 0;
                img[idx + 2] = 0;
            }

            if ((x % 20 < 3) || (y % 20 < 3))
            {
                img[idx + 0] = 255;
                img[idx + 1] = 0;
                img[idx + 2] = 0;
            }
        }
    }
}

// ---------------- LOAD KERNEL ----------------
std::string loadKernel(const char *filename)
{
    std::ifstream file(filename);
    return std::string(
        (std::istreambuf_iterator<char>(file)),
        std::istreambuf_iterator<char>());
}

// ---------------- MAIN ----------------
int main()
{
    int width, height, channels;

    unsigned char *image =
        stbi_load("image.png", &width, &height, &channels, 3);

    if (!image)
    {
        std::cout << "Failed to load image.png\n";
        return 1;
    }

    int imageSize = width * height * 3;

    // -------- CPU TIMING --------
    std::vector<unsigned char> cpuImage(image, image + imageSize);

    auto cpuStart = std::chrono::high_resolution_clock::now();
    sobel_and_grid_cpu(cpuImage.data(), width, height);
    auto cpuEnd = std::chrono::high_resolution_clock::now();

    double cpuTime =
        std::chrono::duration<double, std::milli>(cpuEnd - cpuStart).count();

    // -------- OPENCL SETUP --------
    std::string kernelCode = loadKernel("sobel_and_grid.cl");
    const char *source = kernelCode.c_str();

    cl_platform_id platform;
    clGetPlatformIDs(1, &platform, nullptr);

    cl_device_id device;
    clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &device, nullptr);

    cl_context context =
        clCreateContext(nullptr, 1, &device, nullptr, nullptr, nullptr);

    cl_command_queue queue =
        clCreateCommandQueue(context, device, 0, nullptr);

    cl_mem buffer =
        clCreateBuffer(context,
                       CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR,
                       imageSize,
                       image,
                       nullptr);

    cl_program program =
        clCreateProgramWithSource(context, 1, &source, nullptr, nullptr);

    clBuildProgram(program, 0, nullptr, nullptr, nullptr, nullptr);

    cl_kernel kernel =
        clCreateKernel(program, "sobel_and_grid", nullptr);

    clSetKernelArg(kernel, 0, sizeof(cl_mem), &buffer);
    clSetKernelArg(kernel, 1, sizeof(int), &width);
    clSetKernelArg(kernel, 2, sizeof(int), &height);

    size_t globalSize = width * height;

    // -------- GPU TIMING --------
    auto gpuStart = std::chrono::high_resolution_clock::now();

    clEnqueueNDRangeKernel(queue, kernel, 1,
                           nullptr, &globalSize, nullptr,
                           0, nullptr, nullptr);

    clEnqueueReadBuffer(queue, buffer, CL_TRUE,
                        0, imageSize, image,
                        0, nullptr, nullptr);

    auto gpuEnd = std::chrono::high_resolution_clock::now();

    double gpuTime =
        std::chrono::duration<double, std::milli>(gpuEnd - gpuStart).count();

    // -------- OUTPUT --------
    stbi_write_png("output.png", width, height, 3, image, width * 3);
    stbi_image_free(image);

    std::cout << "CPU time: " << cpuTime << " ms\n";
    std::cout << "GPU time: " << gpuTime << " ms\n";
    std::cout << "Speedup: " << cpuTime / gpuTime << "x\n";
    std::cout << "Saved output.png\n";

    return 0;
}
