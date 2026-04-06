#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "Model/Message.h"
#include "Model/CustomFieldModel.h"
#include "Model/MulticastSender.h"
#include "Controller/MessageController.h"
#include "Model/MessageManager.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;

    // Đăng ký các types cho QML
    qmlRegisterType<Message>("Sender", 1, 0, "Message");
    qmlRegisterType<CustomFieldModel>("Sender", 1, 0, "CustomFieldModel");
    
    // Tạo các model/service
    MessageManager messageManager;
    MulticastSender multicastSender;
    
    // Tạo controller và set dependencies
    MessageController messageController;
    messageController.setMessageManager(&messageManager);
    messageController.setMulticastSender(&multicastSender);
    
    // Đưa vào QML context
    engine.rootContext()->setContextProperty("MessageManager", &messageManager);
    engine.rootContext()->setContextProperty("MessageController", &messageController);

    engine.load(QUrl(QStringLiteral("qrc:/SenderApp/View/Main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;
    return app.exec();
}
