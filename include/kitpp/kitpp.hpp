#ifndef KITPP_KITPP_HPP
#define KITPP_KITPP_HPP

#include "log/log.hpp"
#include "log/scope_timer.hpp"
#include "log/throughput_logger.hpp"
#include "sys/platform.hpp"
#include "sys/version.hpp"

namespace kitpp {
// Declaration for the implementation in src/kitpp.cpp
void printVersion();
}

#endif // KITPP_KITPP_HPP
