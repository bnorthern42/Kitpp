#ifndef KITPP_SCOPE_TIMER_HPP
#define KITPP_SCOPE_TIMER_HPP

#include <string>
#include <format>
#include <source_location>
#include <chrono>
#include "platform.hpp"
#include "log.hpp"

namespace kitpp {

    class ScopeTimer {
    public:
        explicit ScopeTimer(std::string label,
                            std::source_location loc = std::source_location::current())
          : label_(std::move(label)), loc_(loc),
      #if defined(_OPENMP)
            start_(::omp_get_wtime())
      #else
            start_(std::chrono::steady_clock::now())
      #endif
        {}

        ~ScopeTimer() {
#if defined(_OPENMP)
            const double us = (::omp_get_wtime() - start_) * 1e6;
#else
            const auto us = std::chrono::duration_cast<std::chrono::microseconds>(
                std::chrono::steady_clock::now() - start_).count();
#endif
            log::info(std::format("ScopeTimer '{}' elapsed: {} us", label_, static_cast<long long>(us)), loc_);
        }

    private:
        std::string label_;
        std::source_location loc_;
#if defined(_OPENMP)
        double start_;
#else
        std::chrono::time_point<std::chrono::steady_clock> start_;
#endif
    };

} // namespace kitpp

#endif // KITPP_SCOPE_TIMER_HPP
