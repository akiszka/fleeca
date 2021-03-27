#include <CImg.h>
#include <iostream>
#include <thrust/complex.h>

using namespace cimg_library;

using u = std::uint32_t;
using i = std::int32_t;

constexpr u max_iter = 2000;

constexpr u window_x = 1920*4;
constexpr u window_y = 1080*4;

__global__ void generate_set(u win_x, u win_y, u* result, u max_iter);
__device__ double map(double x, double in_min, double in_max, double out_min, double out_max);
__device__ u generate_color(std::uint8_t r, std::uint8_t g, std::uint8_t b);
__device__ u iter_to_color(u iterations, u max_iter);

int main() {
    CImg<unsigned char> theImage(window_x, window_y, 1, 3, 0);

    u* res_arr;
    cudaMallocManaged(&res_arr, sizeof(u)*window_x*window_y*2 /* FIXME: to avoid segfaults I added a *2 to the allocated memory */);

    generate_set<<<24,512>>>(window_x, window_y, res_arr, max_iter);
    cudaDeviceSynchronize();

    for (u x = 0; x < window_x; ++x) {
	for (u y = 0; y < window_y; ++y) {
	    u color = res_arr[x*window_x+y];
	    theImage(x, y, 0) =  color & 0xff;
	    theImage(x, y, 1) = (color>>8) & 0xff;
	    theImage(x, y, 2) = (color>>16) & 0xff;
	}
    }

    cudaFree(res_arr);

    theImage.save("img.png");

    return 0;
}

__global__ void generate_set(u win_x, u win_y, u* result, u max_iter) {
    const thrust::complex<double> julia_c(-0.1f, 0.65f);

    u iter = 0;

    u x_index = blockIdx.x;
    u x_stride = gridDim.x;

    u y_index = threadIdx.x;
    u y_stride = blockDim.x;

    for (u x = x_index; x < win_x; x += x_stride) {
	for(u y = y_index; y < win_y; y += y_stride) {
	    thrust::complex<double> a (
		map(x, 0.0f, win_x, -2.5f, 1.5f),
		map(y, 0.0f, win_y, -1.25f, 1.25f)
		);
	    thrust::complex<double> z = 0;

	    for (iter = 0; abs(z) <= 2 && iter < max_iter; ++iter) {
		z = z*z + a;
	    }

	    result[x*window_x+y] = iter_to_color(iter, max_iter);
	}
    }
}

__device__ u generate_color(std::uint8_t r, std::uint8_t g, std::uint8_t b) {
    return (b<<16) + (g<<8) + r;
}

__device__ u iter_to_color(u iterations, u max_iter) {
    switch (iterations*7/max_iter) {
    case 0:
	return generate_color(255, 0, 0); // red
	break;
    case 1:
    case 2:
	return generate_color(255, 128, 0); // orange
	break;
    case 3:
	return generate_color(0, 255, 0); // green
	break;
    case 4:
	return generate_color(0, 255, 128); // blue-greenish
	break;
    case 5:
	return generate_color(0, 255, 255); // light blue
	break;
    case 6:
	return generate_color(0, 0, 255); // dark blue
	break;
    case 7:
	return generate_color(0, 0, 0);
	break;
    }

    return 0;
}

__device__ double map(double x, double in_min, double in_max, double out_min, double out_max) {
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}