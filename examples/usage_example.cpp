#include <chrono>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

#include "kitpp/kitpp.hpp" // Includes log, scope_timer, etc.

void some_work()
{
    // Use the macro to automatically capture file/line info for the timer
    KITPP_SCOPE_TIMER("some_work");

    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    KITPP_LOG_WARN("This is a warning inside some_work.");
}

int main()
{
    KITPP_LOG_INFO(std::string("Starting application, kitpp version: ") + std::string(kitpp::version_string));

    some_work();

    kitpp::ThroughputLogger tlog("processing_items");
    long long items_processed = 0;
    for (int i = 0; i < 3; ++i) {
        items_processed += 1000;
        std::this_thread::sleep_for(std::chrono::milliseconds(80));
        tlog.record(items_processed);
    }

    int my_var = 42;
    KITPP_LOG_VAR(my_var);

    KITPP_LOG_THREAD_CONTEXT("main thread final check");

    KITPP_LOG_ERROR("Exiting with a simulated error.");

    return 0;
}
