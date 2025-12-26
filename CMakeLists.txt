cmake_minimum_required(VERSION 3.25)

# Project Definition
project(kitpp VERSION 0.1.0 LANGUAGES CXX)

# --- Standard Directory Variables ---
# Defines CMAKE_INSTALL_LIBDIR, CMAKE_INSTALL_INCLUDEDIR, etc.
include(GNUInstallDirs)

# --- Options ---
option(KITPP_BUILD_EXAMPLES "Build usage examples" ON)

# --- Library Target ---
# Defined as INTERFACE (Header-Only).
# Note: If you decide to compile 'kitpp.cpp' into a static lib later,
# change INTERFACE to STATIC and add the source file here.
add_library(kitpp INTERFACE)
add_library(kitpp::kitpp ALIAS kitpp)

# --- Compiler Features ---
# Enforce C++23 for consumers of this library
target_compile_features(kitpp INTERFACE cxx_std_23)

# --- Include Directories ---
target_include_directories(kitpp INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

# --- Dependencies ---
find_package(OpenMP QUIET)
if(OpenMP_CXX_FOUND)
    target_link_libraries(kitpp INTERFACE OpenMP::OpenMP_CXX)
endif()

# --- Examples ---
if(KITPP_BUILD_EXAMPLES)
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/examples/usage_example.cpp")
        add_executable(usage_example examples/usage_example.cpp)
        # Link against the alias to ensure includes/flags are inherited
        target_link_libraries(usage_example PRIVATE kitpp::kitpp)
        # Ensure example uses C++23
        target_compile_features(usage_example PRIVATE cxx_std_23)
    endif()
endif()

# --- Installation & Exporting ---
include(CMakePackageConfigHelpers)

# Define where cmake config files will go (usually lib/cmake/kitpp)
set(KITPP_CMAKE_DIR "${CMAKE_INSTALL_LIBDIR}/cmake/kitpp")

# 1. Generate the Config file (Requires cmake/kitppConfig.cmake.in)
configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/kitppConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/kitppConfig.cmake"
    INSTALL_DESTINATION "${KITPP_CMAKE_DIR}"
    PATH_VARS CMAKE_INSTALL_INCLUDEDIR
)

# 2. Generate the Version file
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/kitppConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

# 3. Install the Library Target
install(TARGETS kitpp
    EXPORT kitppTargets
    # These destinations use GNUInstallDirs variables
    ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
    INCLUDES DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
)

# 4. Install Headers
install(DIRECTORY include/
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
)

# 5. Install the Export Set (defines kitpp::kitpp for others)
install(EXPORT kitppTargets
    FILE kitppTargets.cmake
    NAMESPACE kitpp::
    DESTINATION "${KITPP_CMAKE_DIR}"
)

# 6. Install the Config and Version files
install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/kitppConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/kitppConfigVersion.cmake"
    DESTINATION "${KITPP_CMAKE_DIR}"
)
