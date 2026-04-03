#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "MulticastSender.h"
#include "MessageController.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;

    qmlRegisterType<MulticastSender>("Sender", 1, 0, "MulticastSender");
    MessageController messageController;
    engine.rootContext()->setContextProperty("MessageController" , &messageController);

    engine.load(QUrl(QStringLiteral("qrc:/SenderApp/Main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;
    return app.exec();
}
