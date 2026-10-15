
cmake_minimum_required(VERSION 3.17.0)
set(CMAKE_C_VERSION 23)
set(CMAKE_C_VERSION_REQUIRED true)

project(shared_mem C)
add_library(memshare)
add_executable(tests)

if (WIN32)
    target_sources(memshare PRIVATE
        src/win_memshare.c
        src/memory-allocator.c
    )
else()
    target_sources(memshare PRIVATE
        src/posix_memshare.c
        src/memory-allocator.c
    )
endif()

target_sources(tests PRIVATE
    tests/main.c
)

target_link_libraries(tests PRIVATE memshare)
