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
#include <QFile>
#include <qmath.h>
#include "mainwindow.h"
#include "databasemanager.h"
#include "logindialog.h"

class QTcpSocket;

class Server : public QObject
{
    Q_OBJECT
public:
    explicit Server(MainWindow *, QObject *parent = 0);
    QTcpServer *server;
    QString username;
    QString passwd;
    QString newPass;
    bool isValid;   //are user details valid
private:
    QTcpSocket *tcpSocket;
    QProcess* process;
    bool readRS=false;
    QString rsCode;
    MainWindow *mw;
    DatabaseManager *dbManager;
    bool compiling = false;
signals:
    void dataReceived(QByteArray );
    void drawLabel(QString );
public slots:
    void CreateTcp();
    void ReadTcp();
    void WriteTcp(QByteArray );
    void readStdIn();
    void compileRS(QByteArray);
    void readStdError();
    void createUserDialog();
    void searchUserDialog();


};

#endif // SERVER_H
