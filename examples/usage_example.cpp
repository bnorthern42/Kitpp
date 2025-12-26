#include <iostream>
#include <thread>
#include <chrono>
#include <string>

#include "kitpp/kitpp.hpp" // Includes log, scope_timer, etc.

void some_work() {
    kitpp::ScopeTimer timer("some_work");
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    kitpp::log::warn("This is a warning inside some_work.");
}

int main() {
    kitpp::log::info(std::string("Starting application, kitpp version: ") + kitpp::version_string.data());

    some_work();

    kitpp::ThroughputLogger tlog("processing_items");
    long long items_processed = 0;
    for (int i = 0; i < 3; ++i) {
        items_processed += 1000;
        std::this_thread::sleep_for(std::chrono::milliseconds(80));
        tlog.record(items_processed);
    }

    int my_var = 42;
    kitpp::log::var("my_var", my_var);

    kitpp::log::thread_context("main thread final check");

    kitpp::log::error("Exiting with a simulated error.");

    return 0;
}