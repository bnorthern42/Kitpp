#ifndef KITPP_SCOPE_TIMER_HPP
#define KITPP_SCOPE_TIMER_HPP

#include <string>
#include <utility>
#include <vector>

#include "../sys/platform.hpp" // For OpenMP checks/includes
#include "log.hpp"
#include "TimerCommon.hpp"

namespace kitpp {

// --- ScopeTimer (Console Only, RAII) ---
class ScopeTimer {
public:
    // Accepts location info to log correctly
    ScopeTimer(std::string label, const char* file, int line, const char* func)
        : label_(std::move(label))
        , file_(file)
        , line_(line)
        , func_(func)
    {
#if defined(_OPENMP)
        start_time_ = ::omp_get_wtime();
#else
        start_time_ = std::chrono::steady_clock::now();
#endif
    }

    ~ScopeTimer()
    {
        long long us = get_elapsed_us();
        std::string msg = "ScopeTimer '" + label_ + "' elapsed: " + std::to_string(us) + " us";
        log::detail::log_impl("INFO    ", rang::fg::cyan, msg, file_, line_, func_);
    }

protected:
    long long get_elapsed_us() const
    {
#if defined(_OPENMP)
        return static_cast<long long>((::omp_get_wtime() - start_time_) * 1e6);
#else
        auto now = std::chrono::steady_clock::now();
        return std::chrono::duration_cast<std::chrono::microseconds>(now - start_time_).count();
#endif
    }

    std::string label_;
    const char* file_;
    int line_;
    const char* func_;
#if defined(_OPENMP)
    double start_time_;
#else
    std::chrono::time_point<std::chrono::steady_clock> start_time_;
#endif
};

// --- ScopeTimerFile (Console + CSV, RAII) ---
class ScopeTimerFile : public ScopeTimer {
public:
    ScopeTimerFile(std::string label, const char* file, int line, const char* func)
        : ScopeTimer(std::move(label), file, line, func)
    {
    }

    ~ScopeTimerFile()
    {
        long long us = get_elapsed_us();
        detail::time::log_time_to_file(label_, us, file_, line_, func_);
        // Base destructor logs to console
    }
};

} // namespace kitpp

// Macros to capture location in C++17

#define KITPP_SCOPE_TIMER(label) \
    kitpp::ScopeTimer kitpp_timer_##__LINE__(label, __FILE__, __LINE__, __func__)

#define KITPP_MEASURE_SCOPE(label) \
    kitpp::ScopeTimerFile kitpp_file_timer_##__LINE__(label, __FILE__, __LINE__, __func__)

#endif // KITPP_SCOPE_TIMER_HPP