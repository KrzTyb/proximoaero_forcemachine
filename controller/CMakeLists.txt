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

    include/USBDeviceHandler.hpp
    USBDeviceHandler.cpp

    include/DataSaver.hpp
    DataSaver.cpp
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
        Qt6::Multimedia
        Qt6::Concurrent
)

if(BUILD_ARM)
target_link_libraries(${NAME}
    PUBLIC
        gpiodcxx
)
endif()