set(NAME controller)


set(SOURCES
    include/ForceController.hpp
    ForceController.cpp

    include/MeasureController.hpp
    MeasureController.cpp

    include/GPIOController.hpp
    GPIOController.cpp

    include/GPIOInputs.hpp
    GPIOInputs.cpp

    include/GPIOOutputs.hpp
    GPIOOutputs.cpp

    include/StepMotor.hpp
    StepMotor.cpp
)

add_library(${NAME} ${SOURCES})

target_include_directories(${NAME}
    PUBLIC
        include
)

target_link_libraries(${NAME}
    PUBLIC
        connectors
        helpers
        zmq
        gpiodcxx
)
