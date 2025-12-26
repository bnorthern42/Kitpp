#ifndef KITPP_THROUGHPUT_LOGGER_HPP
#define KITPP_THROUGHPUT_LOGGER_HPP

#include <string>
#include <format>
#include <chrono>
#include "platform.hpp"
#include "log.hpp"


namespace kitpp {

    class ThroughputLogger {
    public:
        explicit ThroughputLogger(std::string label)
          : label_(std::move(label)),
            last_ops_(0),
      #if defined(_OPENMP)
            start_(::omp_get_wtime())
      #else
            start_(std::chrono::steady_clock::now())
      #endif
        {}

        void record(long long ops) {
#if defined(_OPENMP)
            const double elapsed = ::omp_get_wtime() - start_;
#else
            const double elapsed = std::chrono::duration<double>(
                std::chrono::steady_clock::now() - start_).count();
#endif
            const long long ops_since_last_record = ops - last_ops_;
            if (elapsed > 0) {
                const double r = static_cast<double>(ops_since_last_record) / elapsed;
                log::info(std::format("Throughput '{}': {:.2f} ops/sec", label_, r));
            } else {
                log::info(std::format("Throughput '{}': elapsed too short.", label_));
            }
#if defined(_OPENMP)
            start_ = ::omp_get_wtime();
#else
            start_ = std::chrono::steady_clock::now();
#endif
            last_ops_ = ops;
        }

    private:
        std::string label_;
        long long last_ops_;
#if defined(_OPENMP)
        double start_;
#else
        std::chrono::time_point<std::chrono::steady_clock> start_;
#endif
    };

} // namespace kitpp

#endif // KITPP_THROUGHPUT_LOGGER_HPP
