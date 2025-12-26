#include <chrono>
#include <immintrin.h>
#include <iomanip>
#include <iostream>
#include <omp.h>
#include <vector>

namespace kitpp::math {

void axpy_scalar(double alpha, const std::vector<double>& x, std::vector<double>& y)
{
#pragma omp parallel for
    for (size_t i = 0; i < y.size(); ++i) {
        y[i] += alpha * x[i];
    }
}

// 2. Optimized AVX2 Version
// Strategy:
// - Use AVX2 FMA (Fused Multiply-Add) to do 4 operations at once.
// - Unroll loop 4x (16 elements) to pipeline memory requests.
// - Use OpenMP for multi-core memory saturation.
void axpy_avx(double alpha, const std::vector<double>& x, std::vector<double>& y)
{
    size_t n = y.size();

    // Broadcast alpha to a vector: [alpha, alpha, alpha, alpha]
    __m256d v_alpha = _mm256_set1_pd(alpha);

#pragma omp parallel for schedule(static)
    for (size_t i = 0; i < n; i += 16) {
        // Handle boundary cleanup for non-multiples of 16
        if (i + 15 >= n) {
            for (size_t j = i; j < n; ++j) {
                y[j] += alpha * x[j];
            }
            continue;
        }

        // Unroll 4x (Process 16 elements per step)
        // 1. Load X
        __m256d x0 = _mm256_loadu_pd(&x[i]);
        __m256d x1 = _mm256_loadu_pd(&x[i + 4]);
        __m256d x2 = _mm256_loadu_pd(&x[i + 8]);
        __m256d x3 = _mm256_loadu_pd(&x[i + 12]);

        // 2. Load Y
        __m256d y0 = _mm256_loadu_pd(&y[i]);
        __m256d y1 = _mm256_loadu_pd(&y[i + 4]);
        __m256d y2 = _mm256_loadu_pd(&y[i + 8]);
        __m256d y3 = _mm256_loadu_pd(&y[i + 12]);

        // 3. FMA: y = (alpha * x) + y
        y0 = _mm256_fmadd_pd(v_alpha, x0, y0);
        y1 = _mm256_fmadd_pd(v_alpha, x1, y1);
        y2 = _mm256_fmadd_pd(v_alpha, x2, y2);
        y3 = _mm256_fmadd_pd(v_alpha, x3, y3);

        // 4. Store Y
        _mm256_storeu_pd(&y[i], y0);
        _mm256_storeu_pd(&y[i + 4], y1);
        _mm256_storeu_pd(&y[i + 8], y2);
        _mm256_storeu_pd(&y[i + 12], y3);
    }
}

}
