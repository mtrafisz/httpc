cmake_minimum_required (VERSION 3.5)

project (httpc LANGUAGES C)

file (GLOB_RECURSE httpc_SOURCES "source/*.c")
add_library (httpc STATIC
    ${httpc_SOURCES}
)

target_include_directories(httpc PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/third-party
)

option (HTTPC_BUILD_TESTS "Build tests" OFF)
# set(HTTPC_BUILD_TESTS ON CACHE BOOL "Build the tests" FORCE) # to force the option to be on

if (HTTPC_BUILD_TESTS)
    file (GLOB_RECURSE httpc_TESTS "test/*.c")

    foreach(test ${httpc_TESTS})
        get_filename_component(test_name ${test} NAME_WE)
        add_executable(${test_name} ${test})
        target_include_directories(${test_name} PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/include
            ${CMAKE_CURRENT_SOURCE_DIR}/test/include
        )
        target_link_libraries(${test_name} httpc)
        add_test(NAME ${test_name} COMMAND ${test_name})
    endforeach()
endif()

if ( CMAKE_COMPILER_IS_GNUCC OR CMAKE_C_COMPILER_ID MATCHES "Clang" )
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Wpedantic")
elseif ( MSVC )
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /W4")
endif()
