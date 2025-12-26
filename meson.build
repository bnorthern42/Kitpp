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

# --- Include Directories ---
inc = include_directories('include')

# --- Main Library Target ---
# Now compiling src/kitpp.cpp
libkitpp = library('kitpp',
  'src/printVersion.cpp',
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
# We pass 'libkitpp' so pkg-config knows to link against it (-lkitpp)
pkg = import('pkgconfig')
pkg.generate(libkitpp,
  name : 'kitpp',
  description : 'A C++23 library',
  url : 'https://github.com/yourname/kitpp',
  version : meson.project_version()
)

# --- Examples ---
if get_option('build_examples')
  executable('usage_example',
    'examples/usage_example.cpp',
    dependencies : kitpp_dep
  )
endif
