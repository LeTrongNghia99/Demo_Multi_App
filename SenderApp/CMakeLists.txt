cmake_minimum_required(VERSION 3.16)

set(SENDER_APP_NAME SenderApp)

# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Model)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Controller)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/View)

qt_add_executable(${SENDER_APP_NAME}
    main.cpp
    Model/Message.h
    Model/Message.cpp
    Model/MessageManager.h
    Model/MessageManager.cpp
    Model/Customfield.h
    Model/Customfield.cpp
    Model/CustomFieldModel.h
    Model/CustomFieldModel.cpp
    Model/MulticastSender.h
    Model/MulticastSender.cpp
    Model/SendWorkerPool.h
    Model/SendWorkerPool.cpp
    Controller/MessageController.h
    Controller/MessageController.cpp
    View/Main.qml
)

target_link_libraries(${SENDER_APP_NAME}
    PRIVATE Qt6::Core Qt6::Gui Qt6::Qml Qt6::Quick Qt6::Network Qt6::Concurrent
)

qt_add_qml_module(${SENDER_APP_NAME}
    URI SenderApp
    QML_FILES View/Main.qml
    SOURCES
        Model/Message.h
        Model/Message.cpp
        Model/MessageManager.h
        Model/MessageManager.cpp
        Model/Customfield.h
        Model/Customfield.cpp
        Model/CustomFieldModel.h
        Model/CustomFieldModel.cpp
        Model/MulticastSender.h
        Model/MulticastSender.cpp
        Controller/MessageController.h
        Controller/MessageController.cpp
        SOURCES Model/SendWorkerPool.h Model/SendWorkerPool.cpp
)

install(TARGETS ${SENDER_APP_NAME} DESTINATION bin)
