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

    process = new QProcess();
    connect(process,SIGNAL(readyReadStandardOutput()),this,SLOT(readStdIn()));
    connect(process,SIGNAL(readyReadStandardError()),this,SLOT(readStdError()));

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
    static QByteArray apiLevel;
    if(Datacp=="Hi")
    {
        emit dataReceived(Datacp);
        WriteTcp("Hallo ik ben de server\n");
    }
    else if(Datacp.contains("STARTPACKAGE"))
    {
        QList<QByteArray> dataList = Datacp.split(' ');
        qDebug() << "apiLevel = " + dataList[1].trimmed();
        apiLevel = dataList[1].trimmed();
        readRS=true;
        qDebug("Start received");
    }
    else if(Datacp=="give bc\n")
    {
        qDebug("give bc");
        //file uploaded naar de ftp server
        WriteTcp("UPLOADED\n");
    }
    else
    {
        qDebug()<< "DataCP: " + Datacp;
        if(readRS)
        {
            if(Datacp=="ENDPACKAGE\n")
            {
                readRS=false;
                qDebug() << rsCode;
                qDebug("End received");
                QFile file("/home/dries/Desktop/template.rs");
                file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate);
                QTextStream out(&file);
                out << rsCode;
                rsCode.clear();
                file.close();
                compileRS(apiLevel);
            }
            else
            {
                rsCode.append(Datacp);
            }
        }
        else qDebug() << "Data is: " << Datacp;
    }
}
void Server::WriteTcp(QByteArray data)
{
    tcpSocket->write(data);
    tcpSocket->flush();
}
void Server::readStdIn()
{
    qDebug() << process->readAllStandardOutput();
    WriteTcp("Succesful\n");
    qDebug("Succesful");
}
void Server::readStdError()
{
    QByteArray errors =  process->readAllStandardError();
    WriteTcp(errors + "\n");
    qDebug()<< "Error message!" + errors;
}
void Server::compileRS(QByteArray apiLevel)
{
        qDebug() << "Compiling";
        process->start("/home/dries/AndroidDev/adt-bundle-linux-x86_64-20131030/sdk/build-tools/19.0.3/llvm-rs-cc -target-api " + apiLevel + " -o /home/ftpusers/joe/ -p /home/dries/Desktop/ -I /home/dries/AndroidDev/adt-bundle-linux-x86_64-20131030/sdk/build-tools/android-4.4/renderscript/include -I /home/dries/AndroidDev/adt-bundle-linux-x86_64-20131030/sdk/build-tools/android-4.4/renderscript/clang-include /home/dries/Desktop/template.rs");
        qDebug() << "Compiling done";
}
