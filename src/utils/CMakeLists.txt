add_library(Utils dummy.cpp)
target_include_directories(Utils PUBLIC .)

add_subdirectory(threads)
add_subdirectory(mempool)
add_subdirectory(networking)