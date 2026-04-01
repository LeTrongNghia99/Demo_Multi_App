#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "MulticastReceiver.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    // Tạo MulticastReceiver ở C++ và set context property cho QML
    MulticastReceiver receiver;

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("receiver", &receiver);
    engine.load(QUrl(QStringLiteral("qrc:/ReceiverApp/Main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;
    return app.exec();
}
