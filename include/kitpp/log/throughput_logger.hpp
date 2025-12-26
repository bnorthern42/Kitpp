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
    {
#if defined(_OPENMP)
        start_time_ = ::omp_get_wtime();
#else
        start_time_ = std::chrono::steady_clock::now();
#endif
    }

    void log(long long operations_completed)
    {
        double elapsed_sec = 0.0;
#if defined(_OPENMP)
        elapsed_sec = ::omp_get_wtime() - start_time_;
#else
        auto now = std::chrono::steady_clock::now();
        elapsed_sec = std::chrono::duration<double>(now - start_time_).count();
#endif

        if (elapsed_sec > 0) {
            double ops_per_sec = static_cast<double>(operations_completed) / elapsed_sec;
            KITPP_LOG_INFO("ThroughputLogger '" + label_ + "': " + std::to_string(ops_per_sec) + " ops/sec");
        } else {
            KITPP_LOG_INFO("ThroughputLogger '" + label_ + "': Elapsed time too short to calculate ops/sec.");
        }
    }

private:
    std::string label_;
#if defined(_OPENMP)
    double start_time_;
#else
    std::chrono::time_point<std::chrono::steady_clock> start_time_;
#endif
};

} // namespace kitpp

#endif // KITPP_THROUGHPUT_LOGGER_HPP
