include(CTest)

# Only set these if they haven't been set before (to avoid overriding command-line settings)
find_program(VALGRIND "valgrind")
if(VALGRIND)
  set(MEMORYCHECK_COMMAND ${VALGRIND})
  set(MEMORYCHECK_COMMAND_OPTIONS "--trace-children=yes --leak-check=full")
endif()

# Code coverage
option(CODE_COVERAGE "Enable coverage reporting" OFF)

if(CODE_COVERAGE)
    if(NOT CMAKE_BUILD_TYPE MATCHES Debug)
        message(WARNING "Code coverage results with an optimized (non-Debug) build may be misleading")
    endif()

    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} --coverage")
    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} --coverage")
    set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} --coverage")
    set(CMAKE_SHARED_LINKER_FLAGS_DEBUG "${CMAKE_SHARED_LINKER_FLAGS_DEBUG} --coverage")
endif()


add_subdirectory(utils)
add_subdirectory(lfds)
add_subdirectory(logger)