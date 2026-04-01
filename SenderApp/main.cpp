#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "MulticastSender.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    qmlRegisterType<MulticastSender>("Sender", 1, 0, "MulticastSender");

    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/SenderApp/Main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;
    return app.exec();
}
