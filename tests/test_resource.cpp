#include <kitpp/sys/resource.hpp>
#include <cassert>
#include <iostream>

int main() {
    long max_rss = kitpp::resource::get_max_rss_kb();
    std::cout << "Max RSS KB: " << max_rss << std::endl;
    assert(max_rss >= 0 && "get_max_rss_kb() should return a value >= 0");
    return 0;
}
