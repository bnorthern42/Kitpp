#include <chrono>
#include <cstddef>
#include <cstring>
#include <immintrin.h>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <vector>

namespace kitpp::math {

/**
 * @brief Compute the dot product of two double-precision vectors using a scalar loop.
 *
 * Computes:
 * \f[
 *   \sum_{i=0}^{n-1} a_i \cdot b_i
 * \f]
 *
 * @param a Pointer to the first input array of length @p n.
 * @param b Pointer to the second input array of length @p n.
 * @param n Number of elements to process.
 *
 * @return Dot product of @p a and @p b over @p n elements.
 *
 * @pre @p a and @p b point to valid memory containing at least @p n doubles.
 * @pre @p a and @p b may be assumed non-aliasing due to `__restrict__`.
 *
 * @note This version is portable and serves as a correctness/reference baseline.
 */
double dot_scalar(const double* __restrict__ a, const double* __restrict__ b, size_t n)
{
    double sum = 0.0;
    for (size_t i = 0; i < n; ++i) {
        sum += a[i] * b[i];
    }
    return sum;
}

/**
 * @brief Compute the dot product of two double-precision vectors using AVX2/FMA with 4 accumulators.
 *
 * Uses 256-bit SIMD registers (4 doubles per vector) and unrolls the main loop to keep
 * four independent accumulation chains (v0..v3). This reduces dependency chains and can
 * improve throughput on many x86-64 microarchitectures with AVX2 + FMA.
 *
 * @param a Pointer to the first input array of length @p n.
 * @param b Pointer to the second input array of length @p n.
 * @param n Number of elements to process.
 *
 * @return Dot product of @p a and @p b over @p n elements.
 *
 * @pre CPU supports AVX2 and FMA (e.g., compile with `-mavx2 -mfma` or appropriate target flags).
 * @pre @p a and @p b point to valid memory containing at least @p n doubles.
 * @pre Loads use `_mm256_load_pd`, so @p a and @p b must be 32-byte aligned for the vectorized
 *      iterations (or replace loads with `_mm256_loadu_pd` if alignment cannot be guaranteed).
 * @pre @p a and @p b may be assumed non-aliasing due to `__restrict__`.
 *
 * @note Numerical results may differ slightly from the scalar implementation due to
 *       different summation order (floating-point non-associativity).
 */
double dot_avx_4x(const double* __restrict__ a, const double* __restrict__ b, size_t n)
{
    size_t i = 0;

    __m256d v0 = _mm256_setzero_pd();
    __m256d v1 = _mm256_setzero_pd();
    __m256d v2 = _mm256_setzero_pd();
    __m256d v3 = _mm256_setzero_pd();

    for (; i + 15 < n; i += 16) {
        v0 = _mm256_fmadd_pd(_mm256_load_pd(a + i), _mm256_load_pd(b + i), v0);
        v1 = _mm256_fmadd_pd(_mm256_load_pd(a + i + 4), _mm256_load_pd(b + i + 4), v1);
        v2 = _mm256_fmadd_pd(_mm256_load_pd(a + i + 8), _mm256_load_pd(b + i + 8), v2);
        v3 = _mm256_fmadd_pd(_mm256_load_pd(a + i + 12), _mm256_load_pd(b + i + 12), v3);
    }

    __m256d vsum = _mm256_add_pd(_mm256_add_pd(v0, v1), _mm256_add_pd(v2, v3));

    for (; i < n; ++i) {
        double val = a[i] * b[i];
        __m256d tmp = _mm256_set1_pd(val);
        vsum = _mm256_add_pd(vsum, tmp);
    }

    __m128d vlow = _mm256_castpd256_pd128(vsum);
    __m128d vhigh = _mm256_extractf128_pd(vsum, 1);
    __m128d vsum128 = _mm_add_pd(vlow, vhigh);
    __m128d hsum128 = _mm_hadd_pd(vsum128, vsum128);
    return _mm_cvtsd_f64(hsum128);
}

/**
 * @brief Compute the dot product of two double-precision vectors using an AVX2/FMA strategy tuned for AMD Zen 2.
 *
 * Maintains 8 independent accumulators (v0..v7) in the main loop to increase instruction-level
 * parallelism and reduce the impact of FMA latency—an approach that often performs well on Zen 2.
 * After the wide-unrolled loop, accumulators are reduced, then the function continues with a
 * 4-wide SIMD remainder and finally a scalar tail.
 *
 * @param a Pointer to the first input array of length @p n.
 * @param b Pointer to the second input array of length @p n.
 * @param n Number of elements to process.
 *
 * @return Dot product of @p a and @p b over @p n elements.
 *
 * @pre CPU supports AVX2 and FMA (e.g., compile with `-mavx2 -mfma` or appropriate target flags).
 * @pre @p a and @p b point to valid memory containing at least @p n doubles.
 * @pre Loads use `_mm256_load_pd`, so @p a and @p b must be 32-byte aligned for the vectorized
 *      iterations (or replace loads with `_mm256_loadu_pd` if alignment cannot be guaranteed).
 * @pre @p a and @p b may be assumed non-aliasing due to `__restrict__`.
 *
 * @note Numerical results may differ slightly from the scalar implementation due to
 *       different summation order (floating-point non-associativity).
 */
double dot_avx_zen2(const double* __restrict__ a, const double* __restrict__ b, size_t n)
{
    size_t i = 0;

    __m256d v0 = _mm256_setzero_pd();
    __m256d v1 = _mm256_setzero_pd();
    __m256d v2 = _mm256_setzero_pd();
    __m256d v3 = _mm256_setzero_pd();
    __m256d v4 = _mm256_setzero_pd();
    __m256d v5 = _mm256_setzero_pd();
    __m256d v6 = _mm256_setzero_pd();
    __m256d v7 = _mm256_setzero_pd();

    for (; i + 31 < n; i += 32) {
        v0 = _mm256_fmadd_pd(_mm256_load_pd(a + i), _mm256_load_pd(b + i), v0);
        v1 = _mm256_fmadd_pd(_mm256_load_pd(a + i + 4), _mm256_load_pd(b + i + 4), v1);
        v2 = _mm256_fmadd_pd(_mm256_load_pd(a + i + 8), _mm256_load_pd(b + i + 8), v2);
        v3 = _mm256_fmadd_pd(_mm256_load_pd(a + i + 12), _mm256_load_pd(b + i + 12), v3);
        v4 = _mm256_fmadd_pd(_mm256_load_pd(a + i + 16), _mm256_load_pd(b + i + 16), v4);
        v5 = _mm256_fmadd_pd(_mm256_load_pd(a + i + 20), _mm256_load_pd(b + i + 20), v5);
        v6 = _mm256_fmadd_pd(_mm256_load_pd(a + i + 24), _mm256_load_pd(b + i + 24), v6);
        v7 = _mm256_fmadd_pd(_mm256_load_pd(a + i + 28), _mm256_load_pd(b + i + 28), v7);
    }

    __m256d v01 = _mm256_add_pd(v0, v1);
    __m256d v23 = _mm256_add_pd(v2, v3);
    __m256d v45 = _mm256_add_pd(v4, v5);
    __m256d v67 = _mm256_add_pd(v6, v7);

    __m256d v0123 = _mm256_add_pd(v01, v23);
    __m256d v4567 = _mm256_add_pd(v45, v67);
    __m256d vsum = _mm256_add_pd(v0123, v4567);

    for (; i + 3 < n; i += 4) {
        __m256d a_vec = _mm256_load_pd(a + i);
        __m256d b_vec = _mm256_load_pd(b + i);
        vsum = _mm256_fmadd_pd(a_vec, b_vec, vsum);
    }

    __m128d vlow = _mm256_castpd256_pd128(vsum);
    __m128d vhigh = _mm256_extractf128_pd(vsum, 1);
    __m128d vsum128 = _mm_add_pd(vlow, vhigh);
    __m128d hsum128 = _mm_hadd_pd(vsum128, vsum128);
    double final_sum = _mm_cvtsd_f64(hsum128);

    for (; i < n; ++i) {
        final_sum += a[i] * b[i];
    }

    return final_sum;
}
}
