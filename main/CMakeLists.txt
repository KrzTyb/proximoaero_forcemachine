
add_subdirectory(ChartJS)
set(QML_IMPORT_PATH ${CMAKE_CURRENT_BINARY_DIR} CACHE STRING "" FORCE)

set(SOURCES
    ForceMachine.cpp
)

set(QML_SOURCES
    main.qml
    MainView.qml
    ContentPreview.qml
    Key.qml
    KeyDel.qml
    NumericKeyboard.qml
    ChartLine.qml
    DoorPopup.qml
    CalibrationPopup.qml
    ConfigPopup.qml
    MeasurePopup.qml
    ExportPopup.qml

    DebugMenu.qml
)

set(RES
    images/Camera.png
    images/Chart.png
    images/Key_Pressed.png
    images/Key.png
    images/KeyboardTile.png
    images/KeyDel_Pressed.png
    images/KeyDel.png
    images/ScaleInputBackground.png
    images/Start_Button_Pressed.png
    images/Start_Button.png
    images/Tile.png
    images/ProximoAeroLogoWithShadow.png
    images/ProximoAeroLogo.png
)

qt6_add_library(main STATIC)
qt6_add_qml_module(main
    URI Main
    VERSION 1.0
    QML_FILES ${QML_SOURCES}
    RESOURCES ${RES}
    RESOURCE_PREFIX /main
)


target_compile_definitions(main
    PRIVATE
    $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>
)

target_link_libraries(main
    PRIVATE 
    Qt6::Core 
    Qt6::Quick
    Qt6::Multimedia
    chartjsplugin
    controller
)

qt6_add_executable(${PROJECT_NAME}
    ${SOURCES}
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
    Qt6::Core
    Qt6::Quick
    Qt6::Multimedia
    controller
    mainplugin
    chartjsplugin
)

target_compile_definitions(${PROJECT_NAME}
    PRIVATE
    $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>
)