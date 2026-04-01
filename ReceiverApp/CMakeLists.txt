
cmake_minimum_required(VERSION 3.16)

set(RECEIVER_APP_NAME ReceiverApp)

qt_add_executable(${RECEIVER_APP_NAME}
    main.cpp
    MulticastReceiver.cpp
    MulticastReceiver.h
)

qt_add_qml_module(${RECEIVER_APP_NAME}
    URI ReceiverApp
    VERSION 1.0
    QML_FILES
        Main.qml
)

target_link_libraries(${RECEIVER_APP_NAME}
    PRIVATE Qt6::Core Qt6::Gui Qt6::Qml Qt6::Quick Qt6::Network
)

install(TARGETS ${RECEIVER_APP_NAME} DESTINATION bin)
