#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QTcpSocket>
#include <QTcpServer>
#include <QUdpSocket>
#include <QHostInfo>
#include <QHostAddress>
#include <QNetworkInterface>
#include <QNetworkAddressEntry>
#include <QList>
#include <QSignalMapper>
#include <QStringList>
#include <QApplication>
#include <QProcess>

#include "mainwindow.h"

class QTcpSocket;

class Server : public QObject
{
    Q_OBJECT
public:
    explicit Server(MainWindow *, QObject *parent = 0);
    QTcpServer *server;
private:
    QTcpSocket *tcpSocket;
    QProcess* process;

signals:
    void dataReceived(QByteArray );
    void drawLabel(QString );
public slots:
    void CreateTcp();
    void ReadTcp();
    void WriteTcp(QByteArray );
    void readStdIn();
};

#endif // SERVER_H