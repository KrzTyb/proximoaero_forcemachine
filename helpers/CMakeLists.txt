set(NAME helpers)


set(SOURCES
    include/ForceTypes.hpp
    ForceTypes.cpp
)

add_library(${NAME} ${SOURCES})

target_include_directories(${NAME}
    PUBLIC
        include
)

target_link_libraries(${NAME}
    PUBLIC
        Qt6::Core
)
