cmake_minimum_required(VERSION 3.16)

set(SENDER_APP_NAME SenderApp)

qt_add_executable(${SENDER_APP_NAME}
    main.cpp
    MulticastSender.cpp
    MulticastSender.h
    Main.qml
)

target_link_libraries(${SENDER_APP_NAME}
    PRIVATE Qt6::Core Qt6::Gui Qt6::Qml Qt6::Quick Qt6::Network
)

qt_add_qml_module(${SENDER_APP_NAME}
    URI SenderApp
    QML_FILES Main.qml
    SOURCES
    SOURCES
        MessageController.h
        MessageController.cpp
        Message.h
        Message.cpp
)

install(TARGETS ${SENDER_APP_NAME} DESTINATION bin)
