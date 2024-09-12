#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QTcpSocket>
#include <QCoreApplication>
#include <QTextStream>
#include <QTimer>
#include <QDebug>

class MyClient : public QObject {
    Q_OBJECT

public:
    MyClient(QObject *parent = nullptr) : QObject(parent) {
        socket = new QTcpSocket(this);

        // Connect the signals to the slots
        connect(socket, &QTcpSocket::readyRead, this, &MyClient::onReadyRead);
        connect(socket, &QTcpSocket::errorOccurred, this, &MyClient::onErrorOccurred);

        // Attempt to connect to the server
        socket->connectToHost("localhost", 1235);

        if (!socket->waitForConnected(5000)) {
            qCritical() << "Could not connect to server:" << socket->errorString();
            QCoreApplication::quit();
        }
    }

private slots:
    void onReadyRead() {
        while (socket->canReadLine()) {
            QByteArray line = socket->readLine();
            qDebug() << "Received:" << line.trimmed();  // Trim newline characters
        }
    }

    void onErrorOccurred(QAbstractSocket::SocketError socketError) {
        qCritical() << "Socket error:" << socket->errorString();
        QCoreApplication::quit();
    }

private:
    QTcpSocket *socket;
};

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    MyClient client;

    QQmlApplicationEngine engine;
    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);
    engine.loadFromModule("MohsenSimpleTcpSocketClient", "Main");

    return app.exec();
}

#include "main.moc"
