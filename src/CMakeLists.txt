file(GLOB_RECURSE SRCS *.cpp *.c)

configure_file(version.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/include/version.h)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/include)

add_executable(virtualizm-wsproxy ${SRCS})

install(TARGETS virtualizm-wsproxy RUNTIME DESTINATION /usr/bin)
