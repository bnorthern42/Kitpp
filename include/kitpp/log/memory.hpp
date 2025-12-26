#ifndef KITPP_MEMORY_HPP
#define KITPP_MEMORY_HPP

#include <fstream>
#include <mutex>
#include <string>
#include <vector>

namespace kitpp::memory {

// --- 1. Memory Calculation Templates ---

// Default Base Case: Returns sizeof(T)
template <typename T>
size_t get_deep_size(const T& obj)
{
    return sizeof(T);
}

// Specialization: std::vector
template <typename T>
size_t get_deep_size(const std::vector<T>& vec)
{
    return sizeof(std::vector<T>) + (vec.capacity() * sizeof(T));
}

// Specialization: std::string
inline size_t get_deep_size(const std::string& str)
{
    return sizeof(std::string) + str.capacity();
}

// --- 2. File Logger Implementation ---

inline void log_mem_to_file(const char* var_name, const std::string& context,
    size_t bytes, const char* file, int line)
{
    static std::mutex log_mutex;
    std::lock_guard<std::mutex> lock(log_mutex);

    // Open file in Append mode
    std::ofstream outfile("memory_tracker.csv", std::ios::app);

    // Write CSV Header if file is empty/new
    if (outfile.tellp() == 0) {
        outfile << "File,Line,Context,Variable,Bytes,Megabytes\n";
    }

    double mb = static_cast<double>(bytes) / (1024.0 * 1024.0);

    outfile << file << "," << line << "," << context << "," << var_name << ","
            << bytes << "," << mb << "\n";
}

} // namespace kitpp::memory

#define KITPP_LOG_MEM(variable, context)               \
    kitpp::memory::log_mem_to_file(#variable, context, \
        kitpp::memory::get_deep_size(variable),        \
        __FILE__, __LINE__)

#endif // KITPP_MEMORY_HPP
