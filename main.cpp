
#include <QCoreApplication>
#include <QProcess>
#include <QVector>
#include <QObject>
#include <QTimer>
#include <QDebug>
#include <QFileInfo>
#include <QDir>
#include <QGuiApplication>
#include <QQmlApplicationEngine>

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    // Danh sách các app con cần quản lý
    QStringList appPaths = {
        QDir::current().absoluteFilePath("ReceiverApp/ReceiverApp.exe"),
        QDir::current().absoluteFilePath("ReceiverApp_2/ReceiverApp_2.exe"),
        QDir::current().absoluteFilePath("SenderApp/SenderApp.exe")
    };

    QVector<QProcess*> processes;

    // Hàm tắt tất cả các app con
    auto killAll = [&processes]() {
        for (QProcess* proc : processes) {
            if (proc && proc->state() != QProcess::NotRunning) {
                proc->kill();
            }
        }
    };


    // Khởi động các app con và kết nối log
    for (const QString& path : appPaths) {
        if (QFileInfo::exists(path)) {
            QProcess* proc = new QProcess();
            proc->setProgram(path);
            proc->setProcessChannelMode(QProcess::MergedChannels); // Gộp stdout và stderr
            QObject::connect(proc, &QProcess::readyReadStandardOutput, [proc, path]() {
                QByteArray data = proc->readAllStandardOutput();
                QTextStream ts(stdout);
                ts << "[" << QFileInfo(path).baseName() << "] " << QString::fromLocal8Bit(data);
                ts.flush();
            });
            QObject::connect(proc, &QProcess::readyReadStandardError, [proc, path]() {
                QByteArray data = proc->readAllStandardError();
                QTextStream ts(stderr);
                ts << "[" << QFileInfo(path).baseName() << " ERROR] " << QString::fromLocal8Bit(data);
                ts.flush();
            });
            proc->start();
            processes.append(proc);
        } else {
            qWarning() << "App not found:" << path;
        }
    }

    // Nếu bất kỳ app con nào tắt, tắt tất cả app còn lại và thoát
    for (QProcess* proc : processes) {
        QObject::connect(proc, static_cast<void(QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished),
            &app, [&](int, QProcess::ExitStatus) {
                killAll();
                QTimer::singleShot(100, &app, &QCoreApplication::quit);
            });
    }

    // Nếu muốn chạy QML chính của project, bỏ comment đoạn dưới
    // QQmlApplicationEngine engine;
    // QObject::connect(
    //     &engine,
    //     &QQmlApplicationEngine::objectCreationFailed,
    //     &app,
    //     []() { QCoreApplication::exit(-1); },
    //     Qt::QueuedConnection);
    // engine.loadFromModule("Demo_MultiApp", "Main");

    return app.exec();
}
