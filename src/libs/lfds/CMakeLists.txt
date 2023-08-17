add_library(LFDS INTERFACE)
target_link_libraries(LFDS INTERFACE Utils)
target_include_directories(LFDS INTERFACE ./include)
add_subdirectory(include)
