
cmake_minimum_required(VERSION 3.16)
project(DEMO_MULTI_APP LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Qml Quick Network)

# Ensure QML modules are found
set(QML_IMPORT_PATH "${CMAKE_SOURCE_DIR}/SenderApp;${CMAKE_SOURCE_DIR}/ReceiverApp;${CMAKE_SOURCE_DIR}/ReceiverApp_2")
set(QML2_IMPORT_PATH "${QML_IMPORT_PATH}")


# Main app target
add_executable(Demo_Multi_App main.cpp)
target_link_libraries(Demo_Multi_App PRIVATE Qt6::Core Qt6::Gui Qt6::Qml Qt6::Quick Qt6::Network)

add_subdirectory(SenderApp)
add_subdirectory(ReceiverApp)
add_subdirectory(ReceiverApp_2)

# Custom target to run both apps
add_custom_target(run_all
	COMMAND $<TARGET_FILE:SenderApp> & $<TARGET_FILE:ReceiverApp> & $<TARGET_FILE:ReceiverApp_2>
	DEPENDS SenderApp ReceiverApp ReceiverApp_2
	WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)
