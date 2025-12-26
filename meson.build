project('kitpp', 'cpp',
  version : '0.1.0',
  default_options : [
    'cpp_std=c++23',
    'warning_level=3',
    'werror=false',
  ]
)

# --- Compiler Optimizations ---
# These MUST be defined before any targets (library/executable)
if get_option('buildtype') != 'plain'
  # Add SIMD (AVX/FMA) and native optimizations
  add_project_arguments('-march=native', '-mfma', language : 'cpp')
endif

# --- Dependencies ---
# Meson has built-in OpenMP support
omp_dep = dependency('openmp', required : false)

# --- Include Directories ---
inc = include_directories('include')

# --- Main Library Target ---
# Now compiling src/kitpp.cpp
libkitpp = library('kitpp',
  'src/kitpp.cpp',
  include_directories : inc,
  dependencies : omp_dep,
  install : true
)

# --- Internal Dependency ---
# Use this for examples or subprojects
kitpp_dep = declare_dependency(
  include_directories : inc,
  link_with : libkitpp,
  dependencies : omp_dep,
  version : meson.project_version()
)

# --- Installation ---
# Install the headers
# Copies 'include/kitpp' -> '${prefix}/include/kitpp'
install_subdir('include/kitpp', install_dir : get_option('includedir'))

# Generate a pkg-config file (.pc)
pkg = import('pkgconfig')
pkg.generate(libkitpp,
  name : 'kitpp',
  description : 'A C++23 library',
  url : 'https://github.com/yourname/kitpp',
  version : meson.project_version()
)

# --- Examples ---
if get_option('build_examples')

  # List all your example names here (without .cpp extension)
  examples = [
    'usage_example',
    'dot_prod_example',
    'daxpy_example',
  ]

  foreach name : examples
    # Check if file exists first to avoid hard errors if you haven't created the file yet
    if run_command('[', '-f', meson.current_source_dir() + '/examples/' + name + '.cpp', ']', check : false).returncode() == 0
      executable(name,
        'examples/' + name + '.cpp',
        dependencies : kitpp_dep
      )
    endif
  endforeach

endif
