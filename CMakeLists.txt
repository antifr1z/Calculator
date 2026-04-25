cmake_minimum_required(VERSION 3.20)
project(CodingCalculator VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Qml Quick QuickControls2)

qt_standard_project_setup(REQUIRES 6.5)

qt_add_executable(CodingCalculator
    src/main.cpp
    src/CodingEngine.h
    src/CodingEngine.cpp
    src/CodingTreeModel.h
    src/CodingTreeModel.cpp
    src/DeviceManager.h
    src/DeviceManager.cpp
    src/FileIO.h
    src/FileIO.cpp
)

qt_add_qml_module(CodingCalculator
    URI CodingCalculator
    VERSION 1.0
    QML_FILES
        qml/Main.qml
        qml/DeviceSelector.qml
        qml/CodingEditor.qml
        qml/FieldDelegate.qml
        qml/HexInput.qml
        qml/Theme.qml
)

target_link_libraries(CodingCalculator PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Qml
    Qt6::Quick
    Qt6::QuickControls2
)

# Copy config files to build directory
file(GLOB CONFIG_FILES "${CMAKE_SOURCE_DIR}/configs/*.json")
foreach(CONFIG_FILE ${CONFIG_FILES})
    get_filename_component(CONFIG_NAME ${CONFIG_FILE} NAME)
    configure_file(${CONFIG_FILE} ${CMAKE_BINARY_DIR}/configs/${CONFIG_NAME} COPYONLY)
endforeach()

install(TARGETS CodingCalculator
    BUNDLE DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
