#include <kitpp/kitpp.hpp>

#include <cmath>
#include <immintrin.h> // For AVX intrinsics
#include <iomanip>
#include <sstream>
#include <vector>

// --- Dot Product Implementations ---

// 1. Scalar Implementation
double dot_scalar(double* a, double* b, size_t n)
{
    double sum = 0.0;
    for (size_t i = 0; i < n; i++) {
        sum += a[i] * b[i];
    }
    return sum;
}

// 2. AVX (4x unroll per instruction effectively)
double dot_avx_4x(double* a, double* b, size_t n)
{
    __m256d sum = _mm256_setzero_pd();
    // Assuming n is divisible by 4 for simplicity
    for (size_t i = 0; i < n; i += 4) {
        __m256d va = _mm256_load_pd(&a[i]);
        __m256d vb = _mm256_load_pd(&b[i]);
        sum = _mm256_fmadd_pd(va, vb, sum);
    }
    double res[4];
    _mm256_storeu_pd(res, sum);
    return res[0] + res[1] + res[2] + res[3];
}

// 3. Zen2/Modern AVX (8x unroll - 2 accumulators)
// Helps hide FMA latency
double dot_avx_zen2(double* a, double* b, size_t n)
{
    __m256d sum1 = _mm256_setzero_pd();
    __m256d sum2 = _mm256_setzero_pd();

    // Process 8 elements per loop
    size_t i = 0;
    for (; i < n - 7; i += 8) {
        sum1 = _mm256_fmadd_pd(_mm256_load_pd(&a[i]), _mm256_load_pd(&b[i]), sum1);
        sum2 = _mm256_fmadd_pd(_mm256_load_pd(&a[i + 4]), _mm256_load_pd(&b[i + 4]), sum2);
    }

    // Combine accumulators
    sum1 = _mm256_add_pd(sum1, sum2);

    double res[4];
    _mm256_storeu_pd(res, sum1);
    double total = res[0] + res[1] + res[2] + res[3];

    // Handle cleanup scalar
    for (; i < n; i++) {
        total += a[i] * b[i];
    }
    return total;
}

// --- Benchmark Helpers ---

template <typename Func>
double run_benchmark(Func f, double* a, double* b, size_t n, size_t iterations)
{
    auto start = std::chrono::high_resolution_clock::now();
    double sum = 0;
    // Volatile to prevent optimizing away the loop
    volatile double sink = 0;

    for (size_t k = 0; k < iterations; k++) {
        sum += f(a, b, n);
        // Prevent compiler from optimizing away the call
        if (sum == 12345.0)
            sink = sum;
    }
    (void)sink; // Silence "unused variable" warning

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> diff = end - start;
    return diff.count() / iterations;
}

void log_result(const char* name, double time_sec, size_t n_elements)
{
    // 2 arrays * 8 bytes per double
    double total_bytes = (double)n_elements * 8.0 * 2.0;
    double gb = total_bytes / (1024.0 * 1024.0 * 1024.0);
    double bandwidth = gb / time_sec;

    std::stringstream ss;
    ss << std::left << std::setw(12) << name
       << ": " << std::fixed << std::setprecision(6) << time_sec << " s"
       << " | Bandwidth: " << std::setprecision(2) << bandwidth << " GB/s";

    KITPP_LOG_INFO(ss.str());
}

int main()
{
    KITPP_LOG_INFO("Starting Dot Product Benchmark...");
    KITPP_LOG_THREAD_CONTEXT("Main Thread");

    // --- TEST 1: L1 CACHE (32KB Data) ---
    {
        KITPP_SCOPE_TIMER("L1 Cache Test Section");

        size_t n_small = 4096;
        // Use aligned malloc for AVX
        double* a_small = (double*)_mm_malloc(n_small * sizeof(double), 32);
        double* b_small = (double*)_mm_malloc(n_small * sizeof(double), 32);

        for (size_t i = 0; i < n_small; i++) {
            a_small[i] = 1.0;
            b_small[i] = 2.0;
        }

        KITPP_LOG_INFO("--- L1 CACHE TEST (4096 elements) ---");
        size_t iters_small = 100000;

        double t_s = run_benchmark(dot_scalar, a_small, b_small, n_small, iters_small);
        double t_4 = run_benchmark(dot_avx_4x, a_small, b_small, n_small, iters_small);
        double t_8 = run_benchmark(dot_avx_zen2, a_small, b_small, n_small, iters_small);

        // Format and log manually since KITPP_LOG takes a string
        std::stringstream ss;
        ss << "Scalar:    " << t_s * 1e6 << " us";
        KITPP_LOG_INFO(ss.str());

        ss.str("");
        ss << "AVX (4x):  " << t_4 * 1e6 << " us";
        KITPP_LOG_INFO(ss.str());

        ss.str("");
        ss << "Zen2 (8x): " << t_8 * 1e6 << " us";
        KITPP_LOG_INFO(ss.str());

        _mm_free(a_small);
        _mm_free(b_small);
    }

    // --- TEST 2: RAM (100 Million Data) ---
    {
        KITPP_SCOPE_TIMER("RAM Test Section");

        size_t n_large = 100000000;
        KITPP_LOG_INFO("\n--- RAM TEST (100 Million elements) ---");

        double data_size_gb = (double)n_large * 16.0 / (1024.0 * 1024.0 * 1024.0);
        KITPP_LOG_INFO("Data Size: " + std::to_string(data_size_gb) + " GB read per run");

        double* a_large = (double*)_mm_malloc(n_large * sizeof(double), 32);
        double* b_large = (double*)_mm_malloc(n_large * sizeof(double), 32);

// Parallel Init
#pragma omp parallel for
        for (size_t i = 0; i < n_large; i++) {
            a_large[i] = 1.0;
            b_large[i] = 2.0;
        }

        size_t iters_large = 5;
        double t_s = run_benchmark(dot_scalar, a_large, b_large, n_large, iters_large);
        double t_4 = run_benchmark(dot_avx_4x, a_large, b_large, n_large, iters_large);
        double t_8 = run_benchmark(dot_avx_zen2, a_large, b_large, n_large, iters_large);

        log_result("Scalar", t_s, n_large);
        log_result("AVX (4x)", t_4, n_large);
        log_result("Zen2 (8x)", t_8, n_large);

        _mm_free(a_large);
        _mm_free(b_large);
    }

    return 0;
}
