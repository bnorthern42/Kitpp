
#ifndef KITPP_LOG_HPP
#define KITPP_LOG_HPP
#include <iostream>
#include <source_location>
#include <string_view>
#include <string>
#include <typeinfo>

#include "platform.hpp"
#include "external/rang.hpp"

namespace kitpp {

namespace log {

namespace detail {
    // Internal helper to avoid repeating the complex log line formatting
    inline void write_log_entry(
        std::string_view level_name,
        rang::fg level_color,
        std::string_view message,
        std::source_location loc)
    {
        std::clog << rang::style::bold << level_color << level_name << " | " << rang::style::reset
                  << rang::style::bold << "Source_File: " << rang::style::reset << rang::fg::red
                  << loc.file_name() << rang::style::reset << '(' << rang::fgB::cyan << "L"
                  << loc.line() << rang::style::reset << ':' << rang::fgB::green << "C"
                  << loc.column() << rang::style::reset << ")   `" << rang::fg::yellow
                  << loc.function_name() << rang::style::reset << "`: " << message << '\n';
    }
} // namespace detail

inline void log(std::string_view message, // Note: this will be called as kitpp::log::log
                std::source_location loc = std::source_location::current()) {
  std::clog << rang::style::bold << "LOGGER | Source_File: " << rang::style::reset
            << rang::fg::red << loc.file_name() << rang::style::reset << '('
            << rang::fgB::cyan << "L" << loc.line() << rang::style::reset << ':'
            << rang::fgB::green << "C" << loc.column() << rang::style::reset << ")   `"
            << rang::fg::yellow << loc.function_name() << rang::style::reset << "`: "
            << message << '\n';
}

inline void info (std::string_view m, std::source_location l=std::source_location::current()){
  detail::write_log_entry("INFO   ", rang::fg::cyan, m, l);
}
inline void warn (std::string_view m, std::source_location l=std::source_location::current()){
  detail::write_log_entry("WARNING", rang::fg::yellow, m, l);
}
inline void error(std::string_view m, std::source_location l=std::source_location::current()){
  detail::write_log_entry("ERROR  ", rang::fg::red, m, l);
}

template <class T>
inline void var(std::string_view name, const T& value,
                std::source_location l = std::source_location::current()){
  std::clog << rang::style::bold << rang::fg::magenta << "VAR     | " << rang::style::reset
            << rang::style::bold << "Source_File: " << rang::style::reset << rang::fg::red
            << l.file_name() << rang::style::reset << '(' << rang::fgB::cyan << "L"
            << l.line() << rang::style::reset << ':' << rang::fgB::green << "C"
            << l.column() << rang::style::reset << ")   `" << rang::fg::yellow
            << l.function_name() << rang::style::reset << "`: " << rang::fg::magenta
            << "Type: " << rang::style::reset << rang::fg::cyan << typeid(T).name()
            << rang::style::reset << ", " << rang::fg::magenta << "Name: " << rang::style::reset
            << rang::fg::cyan << name << rang::style::reset << ", " << rang::fg::magenta
            << "Value: " << rang::style::reset << rang::fg::cyan << value << rang::style::reset
            << '\n';
}

inline void thread_context(std::string_view label,
                           std::source_location l = std::source_location::current()){
  std::clog << rang::style::bold << rang::fg::cyan << "THREAD  | " << rang::style::reset
            << "pid=" << kitpp::pid()
            << " tid=" << kitpp::tid_string()
            << " omp_tid=" << kitpp::omp_tid()
            << " team=" << kitpp::omp_team()
            << " cpu=" << kitpp::cpu_index()
            << " | " << label
            << " @ " << l.file_name() << ":" << l.line() << " `" << l.function_name() << "`\n";
}

} // namespace log
} // namespace kitpp

#endif // KITPP_LOG_HPP
