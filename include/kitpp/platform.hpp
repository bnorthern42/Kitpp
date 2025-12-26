#ifndef KITPP_PLATFORM_HPP
#define KITPP_PLATFORM_HPP

#include <string>
#include <thread>
#include <sstream>

#if defined(_WIN32)
  #include <windows.h>
#else
  #include <unistd.h>
#endif

#if defined(_OPENMP)
  #include <omp.h>
#endif

namespace kitpp {

    // PID
    inline int pid() {
#if defined(_WIN32)
        return static_cast<int>(::GetCurrentProcessId());
#else
        return static_cast<int>(::getpid());
#endif
    }

    // C++ thread id → string
    inline std::string tid_string() {
        std::ostringstream os;
        os << std::this_thread::get_id();
        return os.str();
    }

    // OpenMP helpers (safe when OpenMP absent)
    inline int omp_tid() {
#if defined(_OPENMP)
        return ::omp_get_thread_num();
#else
        return -1;
#endif
    }
    inline int omp_team() {
#if defined(_OPENMP)
        return ::omp_get_num_threads();
#else
        return 1;
#endif
    }

    // current CPU index (best-effort)
    inline int cpu_index() {
#if defined(_WIN32)
#if defined(__MINGW32__) || defined(__MINGW64__)
        return -1; // GetCurrentProcessorNumber may not be available
#else
        return static_cast<int>(::GetCurrentProcessorNumber());
#endif
#elif defined(__linux__)
#if defined(__GLIBC__)
        return static_cast<int>(::sched_getcpu());
#else
        return -1;
#endif
#else
        return -1;
#endif
    }

} // namespace kitpp

#endif // KITPP_PLATFORM_HPP
