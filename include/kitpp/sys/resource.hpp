#ifndef KITPP_RESOURCE_HPP
#define KITPP_RESOURCE_HPP

#include <fstream>
#include <iostream>
#include <sys/resource.h> // POSIX specific

namespace kitpp::resource {

inline long get_max_rss_kb()
{
    struct rusage usage;
    if (getrusage(RUSAGE_SELF, &usage) == 0) {
        // ru_maxrss is usually in kilobytes (kB) on Linux/macOS
        return usage.ru_maxrss;
    }
    return -1; // Error
}

inline long convert_kb_to_mb(long kb)
{
    return kb / 1024;
}

inline void record_mem_stats(const std::string& file)
{
    long max_mb = convert_kb_to_mb(get_max_rss_kb());
    std::fstream out_f(file, std::ios::in | std::ios_base::out | std::ios_base::app);
    out_f << max_mb << std::endl;
}

} // namespace kitpp::resource

#endif // KITPP_RESOURCE_HPP
