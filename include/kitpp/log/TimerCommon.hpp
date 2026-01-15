#ifndef KITPP_TIMER_COMMON_HPP
#define KITPP_TIMER_COMMON_HPP

#include <chrono>
#include <fstream>
#include <iomanip>
#include <mutex>
#include <sstream>
#include <string>
#include <string_view>

namespace kitpp::detail::time {

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

} // namespace kitpp::detail::time

#endif // KITPP_TIMER_COMMON_HPP