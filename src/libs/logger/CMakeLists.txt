add_subdirectory(src)
add_library(Logger ${LOGGER_SOURCES})

add_subdirectory(include)

target_include_directories(Logger PUBLIC ./include)
target_include_directories(Logger PRIVATE ./src)

target_link_libraries(Logger PUBLIC LFDS)
target_link_libraries(Logger PUBLIC Utils)