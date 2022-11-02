cmake_minimum_required(VERSION 1.18.0)

project(rc-psplash)

if (${CMAKE_BUILD_TYPE} MATCHES "DEBUG|Debug|debug")
add_compile_definitions(-DDEBUG)
endif()

find_package(PkgConfig REQUIRED)
pkg_check_modules(RC openrc REQUIRED)

include_directories(${RC_INCLUDE_DIRS})

add_library(${PROJECT_NAME} SHARED openrc-psplash.c)
target_link_libraries(${PROJECT_NAME} ${RC_LINK_LIBRARIES})
install(TARGETS ${PROJECT_NAME} DESTINATION "/usr/lib/rc/plugins/")
