#ifndef KITPP_THROUGHPUT_LOGGER_HPP
#define KITPP_THROUGHPUT_LOGGER_HPP

#include "../sys/platform.hpp" // For OpenMP checks/includes
#include "log.hpp"
#include <chrono>
#include <string>

namespace kitpp {

class ThroughputLogger {
public:
    explicit ThroughputLogger(std::string label)
        : label_(std::move(label))
        , last_ops_(0)
    {
#if defined(_OPENMP)
        start_time_ = ::omp_get_wtime();
#else
        start_time_ = std::chrono::steady_clock::now();
#endif
    }

    // Renamed 'log' to 'record'
    void record(long long operations_completed)
    {
        double elapsed_sec = 0.0;
#if defined(_OPENMP)
        elapsed_sec = ::omp_get_wtime() - start_time_;
#else
        auto now = std::chrono::steady_clock::now();
        elapsed_sec = std::chrono::duration<double>(now - start_time_).count();
#endif

        // Calculate delta since last record if you want interval throughput,
        // or total throughput. Based on your usage, you are passing total items.
        // Let's assume you want instantaneous throughput (ops/sec) for the total time.
        // Or if you want interval:
        // long long delta_ops = operations_completed - last_ops_;
        // But start_time_ is constructed at start.
        // So this calculates "Average throughput since start".

        if (elapsed_sec > 0) {
            double ops_per_sec = static_cast<double>(operations_completed) / elapsed_sec;
            KITPP_LOG_INFO("ThroughputLogger '" + label_ + "': " + std::to_string(ops_per_sec) + " ops/sec");
        } else {
            KITPP_LOG_INFO("ThroughputLogger '" + label_ + "': Elapsed time too short to calculate ops/sec.");
        }

        // If you want interval-based throughput (resetting timer), uncomment below:
        // start_time_ = ...;
        // last_ops_ = operations_completed;
    }

private:
    std::string label_;
    long long last_ops_;
#if defined(_OPENMP)
    double start_time_;
#else
    std::chrono::time_point<std::chrono::steady_clock> start_time_;
#endif
};

} // namespace kitpp

#endif // KITPP_THROUGHPUT_LOGGER_HPP
