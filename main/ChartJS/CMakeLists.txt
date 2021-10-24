 
set(NAME chartjs)

qt_add_library(${NAME} STATIC)

qt6_add_qml_module(${NAME}
    URI ChartJS
    VERSION 1.0
    QML_FILES
        Chart.qml
        Chart.js
)
