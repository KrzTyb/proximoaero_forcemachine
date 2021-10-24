set(NAME connectors)


set(SOURCES
    include/BackendConnector.hpp
    BackendConnector.cpp
)

add_library(${NAME} ${SOURCES})

target_include_directories(${NAME}
    PUBLIC
        include
)

target_link_libraries(${NAME}
    PUBLIC 
        Qt6::Core
        helpers
)