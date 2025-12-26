project('kitpp', 'cpp',
  version : '0.1.0',
  default_options : [
    'cpp_std=c++23',
    'warning_level=3',
    'werror=false',
  ]
)

# --- Dependencies ---
# Meson has built-in OpenMP support
omp_dep = dependency('openmp', required : false)

# --- Header-Only Interface ---
inc = include_directories('include')

# Define the dependency object for internal use (e.g. by examples)
# This mimics CMake's 'INTERFACE' target
kitpp_dep = declare_dependency(
  include_directories : inc,
  dependencies : omp_dep,
  version : meson.project_version()
)

# --- Installation ---
# Install the headers
# Copies 'include/kitpp' -> '${prefix}/include/kitpp'
install_subdir('include/kitpp', install_dir : get_option('includedir'))

# Generate a pkg-config file (.pc)
# This allows other projects to use: pkg-config --cflags --libs kitpp
pkg = import('pkgconfig')
pkg.generate(
  name : 'kitpp',
  description : 'A C++23 library',
  url : 'https://github.com/yourname/kitpp', # Optional
  version : meson.project_version(),
  # Propagate OpenMP flags to consumers
  dependencies : omp_dep
)

# --- Examples ---
if get_option('build_examples')
  executable('usage_example',
    'examples/usage_example.cpp',
    dependencies : kitpp_dep
  )
endif
