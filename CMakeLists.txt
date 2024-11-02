cmake_minimum_required (VERSION 3.20)
project (httpc C)
set(CMAKE_C_STANDARD 99)

file (GLOB_RECURSE LOCAL_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/source/*.c")
file (GLOB_RECURSE UTILS_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/utils/*.c")
add_library (httpc_a STATIC ${LOCAL_SOURCES} ${UTILS_SOURCES})
add_library (httpc_so SHARED ${LOCAL_SOURCES} ${UTILS_SOURCES})

target_include_directories (httpc_a PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include" "${CMAKE_CURRENT_SOURCE_DIR}/utils")
target_include_directories (httpc_so PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include" "${CMAKE_CURRENT_SOURCE_DIR}/utils")

# tests

set (BUILD_TESTS ON)

file (DOWNLOAD "https://raw.githubusercontent.com/mity/acutest/refs/heads/master/include/acutest.h" "${CMAKE_CURRENT_SOURCE_DIR}/utils/acutest.h")
file (GLOB TEST_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/tests/*.c")

# check if acutest is downloaded:
if (NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/utils/acutest.h")
    message (WARNING "acutest.h not found. Tests will not be built.")
    set (BUILD_TESTS OFF)
endif ()

if (BUILD_TESTS)
    foreach (TEST_SOURCE ${TEST_SOURCES})
        get_filename_component (TEST_NAME ${TEST_SOURCE} NAME_WE)
        add_executable (${TEST_NAME} ${TEST_SOURCE})
        target_link_libraries (${TEST_NAME} httpc_a)
        target_include_directories (${TEST_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include" "${CMAKE_CURRENT_SOURCE_DIR}/utils")
    endforeach ()
endif ()
