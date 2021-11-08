 
set(NAME chartjs)

set(QML_IMPORT_PATH ${CMAKE_CURRENT_BINARY_DIR} CACHE STRING "" FORCE)

qt_add_library(${NAME} STATIC)

qt6_add_qml_module(${NAME}
    URI Main.ChartJS
    VERSION 1.0
    QML_FILES
        ChartView.qml
        Chart.js
    RESOURCE_PREFIX /main
)

target_link_libraries(${NAME}
    PRIVATE 
    Qt6::Core 
    Qt6::Quick
)