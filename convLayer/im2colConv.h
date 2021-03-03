#ifndef IM2COLCONV_H
#define IM2COLCONV_H

#include "../conv_layer.h"
#include "../im2col/gemm_pack.h"
#include "../im2col/gemm_kernel.h"


class ConvIm2colLayer : public ConvLayer {

public:
    ConvIm2colLayer(float *input, float *kernel, float *biasw, size_t ic, size_t ih, size_t iw, size_t oc,
                    size_t kh=3, size_t kw=3, size_t sh=1, size_t sw=1, 
                    size_t pad_left=1, size_t pad_right=1, size_t pad_top=1, size_t pad_bottom=1, size_t g=1, bool bias=0, 
                    int mc=8, int nc=256, int kc=384, int gemm_version=1, int row_batch=8, int col_batch=8,
                    int is_pack_c=0, int mt_pack_b_version=2, int prefetch_a=256, int prefetch_b=256, int prefetch_c=0)
	                : ConvLayer(input, kernel, biasw, ic, ih, iw, oc, kh, kw, sh, sw, pad_left, pad_right, pad_top, pad_bottom, g, bias),
                    mc(mc), nc(nc), kc(kc), gemm_version(gemm_version), row_batch(row_batch), col_batch(col_batch),
                    is_pack_c(is_pack_c), mt_pack_b_version(mt_pack_b_version), prefetch_a(prefetch_a), prefetch_b(prefetch_b), prefetch_c(prefetch_c) {
        
        transform_input_data = static_cast<float*>(malloc(sizeof(float) * output_height * output_width * input_channels * kernel_height * kernel_width));
    }
        
    int Init();
    int Tuning();
    int Forward();

    void im2col();
    void im2col_v1();

    void sgemm(); 
    void GEMM(float* A, float* B, float* C, int M, int N, int K);
    void GEMM_v2(float* A, float* B, float* C, int M, int N, int K);
    void GEMM_multithread(float* A, float* B, float* C, int M, int N, int K);

protected:
    typedef void (*PackA)(int, int, float *, int, float *, int, int, int, const int, const int);
    typedef void (*PackB)(int, int, float *, int, float *, const int, const int);
    typedef void (*PackBMT)(int, int, int, int, float*, int, float*, int, int, const int, const int);
    typedef void (*PackC)(int, int, int, float*, float*, int, const int, const int);
    typedef void (*UnpackC)(int, int, int, float*, float*, int, const int, const int);
    typedef void (*InnerKernel)(int, float *, float *, float *, int, const int, const int, const int);
    typedef void (*InnerKernelForCorner)(int, float *, float *, float *, int);

    PackA pack_a;
    PackB pack_b;
    PackBMT pack_b_mt;
    PackC pack_c;
    UnpackC unpack_c;
    InnerKernel inner_kernel;
    InnerKernelForCorner inner_kernel_for_corner;

    int mc;
    int nc;
    int kc;
    int gemm_version;
    int row_batch;
    int col_batch;
    int is_pack_c;
    int mt_pack_b_version;
    int prefetch_a;
    int prefetch_b;
    int prefetch_c;
    float *transform_input_data;

    static const int GEMM_NO_BLOCKS = 0;
    static const int GEMM_BLOCKS_SINGLE_THREAD = 1;
    static const int GEMM_BLOCKS_MULTI_THREADS = 2;
    static const int GEMM_BLOCKS_MULTI_THREADS_2D = 3;

    void set_pack_a();
    void set_pack_b();
    void set_pack_b_mt();
    void set_pack_c();
    void set_unpack_c();
    void set_inner_kernel();
    void set_inner_kernel_for_corner(int k);

};

#endif 