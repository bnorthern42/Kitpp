#ifndef KITPP_LOG_HPP
#define KITPP_LOG_HPP

#include <iostream>
#include <sstream>
#include <string>
#include <string_view>
#include <typeinfo>

#include "../external/rang.hpp"
#include "../sys/platform.hpp"

namespace kitpp::log {

namespace detail {
    // Internal implementation matching your logger.hpp
    inline void log_impl(const std::string_view level_str,
        const rang::fg level_color, const std::string_view message,
        const char* file, int line, const char* func)
    {
        std::clog << rang::style::bold << level_color << level_str << "| "
                  << rang::style::reset << rang::style::bold
                  << "Source_File: " << rang::style::reset << rang::fg::red << file
                  << rang::style::reset << '(' << rang::fgB::cyan << "L" << line
                  << rang::style::reset << ")   `" << rang::fg::yellow << func
                  << rang::style::reset << "`: " << message << '\n';
    }

    inline void log_var_impl(const std::string_view var_name, const std::string& type_name,
        const std::string& value_str,
        const char* file, int line, const char* func)
    {
        std::clog << rang::style::bold << rang::fg::magenta << "VAR     | "
                  << rang::style::reset << rang::style::bold
                  << "Source_File: " << rang::style::reset << rang::fg::red << file
                  << rang::style::reset << '(' << rang::fgB::cyan << "L" << line
                  << rang::style::reset << ")   `" << rang::fg::yellow << func
                  << rang::style::reset << "`: " << rang::fg::magenta
                  << "Type: " << rang::style::reset << rang::fg::cyan
                  << type_name << rang::style::reset << ", "
                  << rang::fg::magenta << "Name: " << rang::style::reset
                  << rang::fg::cyan << var_name << rang::style::reset << ", "
                  << rang::fg::magenta << "Value: " << rang::style::reset
                  << rang::fg::cyan << value_str << rang::style::reset << '\n';
    }

    inline void thread_context_impl(std::string_view label,
        const char* file, int line, const char* func)
    {
        std::clog << rang::style::bold << rang::fg::cyan << "THREAD  | " << rang::style::reset
                  << "pid=" << kitpp::pid()
                  << " tid=" << kitpp::tid_string()
                  << " omp_tid=" << kitpp::omp_tid()
                  << " team=" << kitpp::omp_team()
                  << " cpu=" << kitpp::cpu_index()
                  << " | " << label
                  << " @ " << file << ":" << line << " `" << func << "`\n";
    }
} // namespace detail

// --- User-Facing Macros (C++17) ---

#define KITPP_LOG_INFO(message) \
    kitpp::log::detail::log_impl("INFO    ", rang::fg::cyan, message, __FILE__, __LINE__, __func__)

#define KITPP_LOG_WARN(message) \
    kitpp::log::detail::log_impl("WARNING ", rang::fg::yellow, message, __FILE__, __LINE__, __func__)

#define KITPP_LOG_ERROR(message) \
    kitpp::log::detail::log_impl("ERROR   ", rang::fg::red, message, __FILE__, __LINE__, __func__)

#define KITPP_LOG_THREAD_CONTEXT(label) \
    kitpp::log::detail::thread_context_impl(label, __FILE__, __LINE__, __func__)

// Helper to genericize value printing
template <typename T>
inline void log_var_helper(const std::string_view name, const T& value, const char* file, int line, const char* func)
{
    // Use stream to string conversion to support non-string types safely
    std::ostringstream ss;
    ss << value;
    kitpp::log::detail::log_var_impl(name, typeid(T).name(), ss.str(), file, line, func);
}

#define KITPP_LOG_VAR(variable) \
    kitpp::log::log_var_helper(#variable, variable, __FILE__, __LINE__, __func__)

} // namespace kitpp::log

#endif // KITPP_LOG_HPP
