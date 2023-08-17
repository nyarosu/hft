include(CTest)

# Only set these if they haven't been set before (to avoid overriding command-line settings)
find_program(VALGRIND "valgrind")
if(VALGRIND)
  set(MEMORYCHECK_COMMAND ${VALGRIND})
  set(MEMORYCHECK_COMMAND_OPTIONS "--trace-children=yes --leak-check=full")
endif()

add_subdirectory(utils)
add_subdirectory(lfds)
add_subdirectory(logger)