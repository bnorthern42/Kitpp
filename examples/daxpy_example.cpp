#include <kitpp/kitpp.hpp>
#include <kitpp/math/DAXPY.hpp> // Assumes DAXPY.hpp is in include/kitpp/math/

#include <chrono>
#include <iomanip>
#include <fmt/format.h>
#include <string>
#include <vector>

using namespace kitpp::math;

int main()
{
    KITPP_LOG_INFO("Starting DAXPY Benchmark...");
    KITPP_LOG_THREAD_CONTEXT("Main Thread");

    size_t n = 100000000; // 100 Million elements
    // 3 arrays accessed (Read X, Read Y, Write Y) * 8 bytes per double
    double data_size_gb = (double)n * 8.0 * 3.0 / (1024.0 * 1024.0 * 1024.0);

    KITPP_LOG_INFO(fmt::format("Initializing Vectors with {} elements...", n));

    // Initialize vectors
    std::vector<double> x(n, 1.0);
    std::vector<double> y(n, 2.0);
    std::vector<double> y_copy = y; // Backup for second run
    double alpha = 0.5;

    KITPP_LOG_INFO(fmt::format("Data Processed: {:.4f} GB per run", data_size_gb));

    // --- Scalar Test ---
    {
        KITPP_SCOPE_TIMER("Scalar DAXPY");
        auto start = std::chrono::high_resolution_clock::now();

        axpy_scalar(alpha, x, y);

        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> t_scalar = end - start;

        double bw_scalar = data_size_gb / t_scalar.count();

        KITPP_LOG_INFO(fmt::format("Scalar: {} s | Bandwidth: {} GB/s", t_scalar.count(), bw_scalar));

        // Store scalar time for speedup calc
        // (Re-calculate or store in variable scope if needed, here just keeping structure)
    }

    // --- AVX Test ---
    {
        y = y_copy; // Reset Y
        KITPP_SCOPE_TIMER("AVX DAXPY");

        auto start = std::chrono::high_resolution_clock::now();

        axpy_avx(alpha, x, y);

        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> t_avx = end - start;

        double bw_avx = data_size_gb / t_avx.count();

        KITPP_LOG_INFO(fmt::format("AVX:    {} s | Bandwidth: {} GB/s", t_avx.count(), bw_avx));

        // Calculate speedup roughly (re-running scalar logic conceptually or storing times)
        // For exact print match to your snippet, we need the scalar time variable accessible.
        // I'll grab the scalar time from a previous run or re-measure if strict structure needed.
        // To keep it clean, let's just use variables:
    }

    // Re-running logic cleanly to ensure variables exist for the final print:
    y = y_copy;

    // 1. Scalar Run
    auto start_s = std::chrono::high_resolution_clock::now();
    axpy_scalar(alpha, x, y);
    auto end_s = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> t_scalar = end_s - start_s;

    // 2. AVX Run
    y = y_copy;
    auto start_a = std::chrono::high_resolution_clock::now();
    axpy_avx(alpha, x, y);
    auto end_a = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> t_avx = end_a - start_a;

    KITPP_LOG_INFO(fmt::format("Speedup: {}x", t_scalar.count() / t_avx.count()));

    return 0;
}
