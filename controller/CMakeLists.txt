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
    
    measure/include/ITransport.hpp
    measure/include/TransportDelegate.hpp

    measure/src/UartBroker.hpp
    measure/src/UartBroker.cpp

    measure/include/TransportBuilder.hpp
    measure/src/TransportBuilder.cpp

    measure/include/MeasureBroker.hpp
    measure/src/MeasureBroker.cpp
    
    include/MeasureCalculator.hpp
    MeasureCalculator.cpp

    VideoController.cpp
)

if (BUILD_PC)
    set(SOURCES ${SOURCES} measure/src/measure.cpp)
endif()

add_library(${NAME} ${SOURCES})

target_include_directories(${NAME}
    PUBLIC
        include
        measure/include
)

target_link_libraries(${NAME}
    PUBLIC
        connectors
        helpers
        zmq
        Qt6::Multimedia
        Qt6::Concurrent
    PRIVATE
        Qt6::SerialPort
)

if(BUILD_ARM)
target_link_libraries(${NAME}
    PUBLIC
        gpiodcxx
)
endif()