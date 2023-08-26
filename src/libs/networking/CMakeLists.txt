add_subdirectory(src)
add_library(Networking ${NETWORKING_SOURCES})

target_include_directories(Networking PUBLIC include)

target_link_libraries(Networking PRIVATE Utils)
target_link_libraries(Networking PUBLIC Logger)