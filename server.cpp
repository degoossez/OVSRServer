#include "server.h"

Server::Server(MainWindow * w, QObject *parent) :
    QObject(parent)
{

    server = new QTcpServer(this);
    connect(this,SIGNAL(drawLabel(QString)),w,SLOT(drawLabel(QString)));
    if(!server->listen(QHostAddress::Any,64000))
    {
        emit drawLabel("<ERROR> Server is unable to listen on port 64000!\n");
    }
    else
    {
        emit drawLabel("<INFO> Server started on port 64000.\n <INFO> Waiting for client.");
    }

    connect(server,SIGNAL(newConnection()),this,SLOT(CreateTcp()));
    connect(this,SIGNAL(dataReceived(QByteArray)),w,SLOT(changeUI(QByteArray)));
}
void Server::CreateTcp()
{
    //MainWindow w;
    tcpSocket = new QTcpSocket;
    tcpSocket = server->nextPendingConnection();
    connect(tcpSocket,SIGNAL(readyRead()),this,SLOT(ReadTcp()));
    qDebug("TCP connection made with a client");
    emit drawLabel("<INFO> TCP connection made with a client");


}
void Server::ReadTcp()
{
    //get data from client
    QByteArray Datacp = tcpSocket->readAll();

    if(Datacp=="Hi")
    {
        qDebug() << "Hi ontvangen!";
        emit dataReceived(Datacp);
    }
    else
    {
        qDebug() << "Data is: " << Datacp;
    }
//    process = new QProcess();
//    qDebug() << "Start reading:";
//    process->start("./llvm-rs-cc -target-api 18 -o /home/dries/Desktop/ -p /home/dries/Desktop/ -I /home/dries/AndroidDev/adt-bundle-linux-x86_64-20131030/sdk/build-tools/android-4.4/renderscript/include -I /home/dries/AndroidDev/adt-bundle-linux-x86_64-20131030/sdk/build-tools/android-4.4/renderscript/clang-include /home/dries/Desktop/template.rs");
//./llvm-rs-cc -target-api 18 -o /home/dries/Desktop/ -p /home/dries/Desktop/ -I /home/dries/AndroidDev/adt-bundle-linux-x86_64-20131030/sdk/build-tools/android-4.4/renderscript/include -I /home/dries/AndroidDev/adt-bundle-linux-x86_64-20131030/sdk/build-tools/android-4.4/renderscript/clang-include /home/dries/Desktop/template.rs
//    connect(process,SIGNAL(readyReadStandardOutput()),this,SLOT(readStdIn()));
}
void Server::WriteTcp(QByteArray data)
{
    qDebug("writing");
    tcpSocket->write(data);
}
void Server::readStdIn()
{
    qDebug() << process->readAllStandardOutput();
}
