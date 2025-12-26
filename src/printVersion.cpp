#include "kitpp/kitpp.hpp"
#include <iostream>

namespace kitpp {

void printVersion()
{
    std::cout << "kitpp version: " << version_string << std::endl;
}

} // namespace kitpp
