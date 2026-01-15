#ifndef KITPP_MANUAL_TIMER_HPP
#define KITPP_MANUAL_TIMER_HPP

#include <string>
#include <utility>

#include "../sys/platform.hpp" // For OpenMP checks/includes
#include "log.hpp"
#include "TimerCommon.hpp"

namespace kitpp {

// --- ManualTimer (Console Only, Explicit Control) ---
class ManualTimer {
public:
    ManualTimer(std::string label, const char* file, int line, const char* func)
        : label_(std::move(label))
        , file_(file)
        , line_(line)
        , func_(func)
        , is_running_(false)
    {
        // Initializing start_time_ to default
#if defined(_OPENMP)
        start_time_ = 0.0;
#else
        start_time_ = std::chrono::steady_clock::time_point();
#endif
    }

    virtual ~ManualTimer()
    {
        if (is_running_) {
            stop();
        }
    }

    void start() {
        if (is_running_) return;

        is_running_ = true;
#if defined(_OPENMP)
        start_time_ = ::omp_get_wtime();
#else
        start_time_ = std::chrono::steady_clock::now();
#endif
    }

    // Stops the timer and logs the result via virtual on_stop
    void stop() {
        if (!is_running_) return;

        long long us = get_elapsed_us();
        on_stop(us); 
        
        is_running_ = false;
    }

    // Stops if running, then starts fresh. Good for loops.
    void restart() {
        if (is_running_) {
            stop();
        }
        start();
    }

    bool is_running() const {
        return is_running_;
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

    // Virtual hook to allow derived classes (File) to add logging behavior
    virtual void on_stop(long long us) {
        std::string msg = "ManualTimer '" + label_ + "' elapsed: " + std::to_string(us) + " us";
        log::detail::log_impl("INFO    ", rang::fg::cyan, msg, file_, line_, func_);
    }

    std::string label_;
    const char* file_;
    int line_;
    const char* func_;
    bool is_running_;

#if defined(_OPENMP)
    double start_time_;
#else
    std::chrono::time_point<std::chrono::steady_clock> start_time_;
#endif
};

// --- ManualTimerFile (Console + CSV, Explicit Control) ---
class ManualTimerFile : public ManualTimer {
public:
    using ManualTimer::ManualTimer; // Inherit constructor

protected:
    void on_stop(long long us) override {
        // Log to CSV first
        detail::time::log_time_to_file(label_, us, file_, line_, func_);
        // Then log to console via base
        ManualTimer::on_stop(us);
    }
};

} // namespace kitpp

// Macros to capture location in C++17

#define CREATE_MANUAL_TIMER(label) \
    kitpp::ManualTimer(label, __FILE__, __LINE__, __func__)

#define KITPP_MEASURE_MANUAL(label) \
    kitpp::ManualTimerFile(label, __FILE__, __LINE__, __func__)

#endif // KITPP_MANUAL_TIMER_HPP