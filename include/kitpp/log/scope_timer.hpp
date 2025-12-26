#ifndef KITPP_SCOPE_TIMER_HPP
#define KITPP_SCOPE_TIMER_HPP

#include <chrono>
#include <fstream>
#include <iomanip>
#include <mutex>
#include <sstream>
#include <string>
#include <vector>

#include "../sys/platform.hpp" // For OpenMP checks/includes
#include "log.hpp"

namespace kitpp {

namespace detail::time {
    // Gets current wall-clock time for the "Timestamp" column
    inline std::string get_current_timestamp()
    {
        using namespace std::chrono;
        auto now = system_clock::now();
        auto in_time_t = system_clock::to_time_t(now);
        auto ms = duration_cast<milliseconds>(now.time_since_epoch()) % 1000;

        std::stringstream ss;
        std::tm tm_buf;
#if defined(_WIN32)
        localtime_s(&tm_buf, &in_time_t);
#else
        localtime_r(&in_time_t, &tm_buf);
#endif

        ss << std::put_time(&tm_buf, "%Y-%m-%d %H:%M:%S");
        ss << "." << std::setfill('0') << std::setw(3) << ms.count();
        return ss.str();
    }

    inline std::string format_duration(long long duration_us)
    {
        using namespace std::chrono;
        auto d = microseconds(duration_us);
        auto h = duration_cast<hours>(d);
        d -= h;
        auto m = duration_cast<minutes>(d);
        d -= m;
        auto s = duration_cast<seconds>(d);
        d -= s;
        auto ms = duration_cast<milliseconds>(d);

        std::stringstream ss;
        ss << std::setfill('0') << std::setw(2) << h.count() << ":"
           << std::setfill('0') << std::setw(2) << m.count() << ":"
           << std::setfill('0') << std::setw(2) << s.count() << "."
           << std::setfill('0') << std::setw(3) << ms.count();
        return ss.str();
    }

    inline void log_time_to_file(const std::string_view scope,
        long long duration_us, const char* file, int line,
        const char* func)
    {
        static std::mutex log_mutex;
        std::lock_guard<std::mutex> lock(log_mutex);

        std::ofstream outfile("speed_tracker.csv", std::ios::app);

        if (outfile.tellp() == 0) {
            outfile << "Timestamp,Scope,File,Function,Line,Duration_us,Duration_Seconds,Duration_Pretty\n";
        }

        outfile << get_current_timestamp() << ","
                << scope << "," << file << "," << func << "," << line << ","
                << duration_us << ","
                << (static_cast<double>(duration_us) / 1000000.0) << ","
                << format_duration(duration_us) << "\n";
    }
} // namespace detail::time

// --- ScopeTimer (Console Only) ---
class ScopeTimer {
public:
    // Now accepts location info to log correctly
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
        // Use log_impl directly to pass the captured file/line of the scope,
        // rather than the file/line of this destructor.
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

// --- ScopeTimerFile (Console + CSV) ---
class ScopeTimerFile : public ScopeTimer {
public:
    // Inherit constructor logic
    ScopeTimerFile(std::string label, const char* file, int line, const char* func)
        : ScopeTimer(std::move(label), file, line, func)
    {
    }

    ~ScopeTimerFile()
    {
        // Note: The base ScopeTimer destructor will run after this, handling the Console log.
        long long us = get_elapsed_us();

        // Log to CSV
        detail::time::log_time_to_file(label_, us, file_, line_, func_);
    }
};

} // namespace kitpp

// Macros to capture location in C++17

#define KITPP_SCOPE_TIMER(label) \
    kitpp::ScopeTimer kitpp_timer_##__LINE__(label, __FILE__, __LINE__, __func__)

#define KITPP_MEASURE_SCOPE(label) \
    kitpp::ScopeTimerFile kitpp_file_timer_##__LINE__(label, __FILE__, __LINE__, __func__)

#endif // KITPP_SCOPE_TIMER_HPP
