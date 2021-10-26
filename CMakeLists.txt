cmake_minimum_required(VERSION 3.16)

project(Proximo_Aero_ForceMachine VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)

find_package(Qt6 6.2 COMPONENTS Core Quick Multimedia REQUIRED)


# Add directories
add_subdirectory(helpers)
add_subdirectory(main)
add_subdirectory(connectors)
add_subdirectory(controller)

install(TARGETS ${PROJECT_NAME} DESTINATION bin)