cmake_minimum_required(VERSION 3.16)

project(Proximo_Aero_ForceMachine VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)

set(BUILD_ARM 1 CACHE STRING "")
set(BUILD_PC 0 CACHE STRING "")

message(STATUS "BUILD_ARM: ${BUILD_ARM}")
message(STATUS "BUILD_PC: ${BUILD_PC}")

configure_file (
  "${PROJECT_SOURCE_DIR}/config.h.in"
  "${PROJECT_BINARY_DIR}/config.h"
)

include_directories("${PROJECT_BINARY_DIR}")

find_package(Qt6 6.2 COMPONENTS Core Quick Multimedia Concurrent REQUIRED)

set(QT_QML_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/main)

# Add directories
add_subdirectory(helpers)
add_subdirectory(main)
add_subdirectory(connectors)
add_subdirectory(controller)

install(TARGETS ${PROJECT_NAME} DESTINATION bin)