add_library(Utils INTERFACE)
target_include_directories(Utils INTERFACE .)

add_subdirectory(threads)
add_subdirectory(mempool)
add_subdirectory(networking)