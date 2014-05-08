#include "server.h"

Server::Server(MainWindow * w, QObject *parent) :
    QObject(parent)
{
    server = new QTcpServer(this);

    mw = w;
    dbManager = new DatabaseManager(this);

    connect(mw->openDB,SIGNAL(clicked()),dbManager,SLOT(openDB()));
    connect(mw->deleteDB,SIGNAL(clicked()),dbManager, SLOT(deleteDB()));
    connect(mw->createTabel,SIGNAL(clicked()),dbManager, SLOT( createUsersTable()));
    connect(mw->createUser, SIGNAL(clicked()),this,SLOT(createUserDialog()));
    connect(mw->searchUser, SIGNAL(clicked()),this,SLOT(searchUserDialog()));
    connect(mw->showDatabase,SIGNAL(clicked()),dbManager, SLOT(printDB()));



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

    dbManager->openDB();

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
        qDebug("Start received");

        username = "";
        passwd = "";
        isValid = false;

        QList<QByteArray> dataList = Datacp.split(' ');
        username = dataList[1].trimmed();
        passwd = dataList[2].trimmed();

        qDebug() << "pass = " + passwd;

        if(dbManager->getUser(username,passwd.toUtf8()))
        {
            qDebug() << "apiLevel = " + dataList[3].trimmed();
            apiLevel = dataList[3].trimmed();
            readRS=true;
            isValid = true;
        }
        else
        {
            WriteTcp("login error\n");;
            readRS=true;
        }


    }
    else if(Datacp=="give bc\n")
    {
        qDebug("give bc");
        //file uploaded naar de ftp server
        WriteTcp("UPLOADED\n");
    }
    else if(Datacp.contains("LOGIN"))
    {
        qDebug() << "inside LOGIN: " + Datacp;

        if(Datacp.contains("ENDLOGIN"))
        {
            qDebug() << "user login request";
            QList<QByteArray> dataList = Datacp.split(' ');

            QString username = dataList[1].trimmed();
            QString passwd = dataList[2].trimmed();            

            if(dbManager->getUser(username,passwd.toUtf8()))
            {
                WriteTcp("login ok\n");

            }
            else
            {
                WriteTcp("login error\n");
            }



        }

    }
    else if(Datacp.contains("ACCOUNT"))
    {
        qDebug() << "inside ACCOUNT: " + Datacp;

        if(Datacp.contains("ENDACCOUNT"))
        {
            qDebug() << "Account create request";

            QList<QByteArray> dataList = Datacp.split(' ');

            QString username = dataList[1].trimmed();
            newPass = dataList[2].trimmed();

            //check if username already exists
            if(dbManager->getUserName(username))
            {
                //username is already in use
                qDebug() << "account already in use";
                WriteTcp("acount error\n");
            }
            else
            {
                qDebug() << "create account";
                qDebug() << "vlak voor " + newPass;
                //create database entry
                dbManager->insertUser(username,newPass.toUtf8());

                //create ftp user
                process->start("mkdir /home/ftpusers/" + username);
                process->waitForFinished();
                process->start("pure-pw useradd " + username + "  -u ftpuser -d /home/ftpusers/" + username);
                process->waitForFinished();
                process->start("pure-pw mkdb");
                process->waitForFinished();

                WriteTcp("acount created\n");

            }

        }
    }
    else
    {
        qDebug()<< "DataCP: " + Datacp;
        if(readRS)
        {
            if(Datacp.contains("ENDPACKAGE\n"))
            {
                qDebug() << rsCode;
                qDebug("End received");
                readRS=false;

                if(isValid)
                {
                    QFile file("/home/ftpusers/" + username + "/template.rs");
                    if(!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate))
                        qDebug() << "cannot find file";
                    QTextStream out(&file);
                    out << rsCode;
                    file.close();
                    compiling = true;
                    compileRS(apiLevel);

                }
                rsCode.clear();

            }
            else
            {
                rsCode.append(Datacp);
            }
        }
        //else qDebug() << "Data is: " << Datacp;
    }
}
void Server::WriteTcp(QByteArray data)
{
    tcpSocket->write(data);
    tcpSocket->flush();
}
void Server::readStdIn()
{
    QString str = process->readAllStandardOutput();
    qDebug() << str;

    if(compiling)
    {
        WriteTcp("Succesful\n");
        compiling = false;
    }

    if(str.contains("Password:"))
    {
        QByteArray byteArray;
        const char *cstr;
        int written;

        byteArray = newPass.toUtf8();
        cstr = byteArray.constData();

        qDebug() << "debug 1 " << cstr;
        if(written = process->write(cstr) < 0)
            qDebug() << "error writing process";
        process->write("\n");
        process->waitForBytesWritten();
        qDebug() << "bytes written " + QString::number(written);
    }
    else if(str.contains("Enter it again"))
    {
        QByteArray byteArray;
        const char *cstr;
        int written;

        byteArray = newPass.toUtf8();
        cstr = byteArray.constData();

        qDebug() << "debug 2 " << cstr;
        if(written = process->write(cstr) < 0)
            qDebug() << "error writing process";
        process->write("\n");
        process->waitForBytesWritten();
        qDebug() << "bytes written " + QString::number(written);
    }
    else
    {
        //WriteTcp("Succesful\n");
        qDebug("Succesful");

    }
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
        qDebug() << "/home/koen/Eindwerk-Eclipse/adt-bundle-linux-x86-20131030/sdk/build-tools/19.0.3/llvm-rs-cc -target-api " + apiLevel + " -o /home/ftpusers/" + username + " -p /home/ftpusers/" + username + " -I /home/koen/Eindwerk-Eclipse/adt-bundle-linux-x86-20131030/sdk/build-tools/android-4.4/renderscript/include -I /home/koen/Eindwerk-Eclipse/adt-bundle-linux-x86-20131030/sdk/build-tools/android-4.4/renderscript/clang-include /home/ftpusers/" + username + "/template.rs";
        process->start("/home/koen/Eindwerk-Eclipse/adt-bundle-linux-x86-20131030/sdk/build-tools/19.0.3/llvm-rs-cc -target-api " + apiLevel + " -o /home/ftpusers/" + username + " -p /home/ftpusers/" + username + " -I /home/koen/Eindwerk-Eclipse/adt-bundle-linux-x86-20131030/sdk/build-tools/android-4.4/renderscript/include -I /home/koen/Eindwerk-Eclipse/adt-bundle-linux-x86-20131030/sdk/build-tools/android-4.4/renderscript/clang-include /home/ftpusers/" + username + "/template.rs");

        qDebug() << "Compiling done";
}

void Server::createUserDialog()
{
    LoginDialog* loginDialog = new LoginDialog();
    connect( loginDialog,
                     SIGNAL(acceptLogin(QString,QByteArray)),
                     dbManager,
                     SLOT(insertUser(QString,QByteArray)));
    loginDialog->exec();

}

void Server::searchUserDialog() {
    LoginDialog* loginDialog = new LoginDialog();
    connect( loginDialog,
                     SIGNAL(acceptLogin(QString,QByteArray)),
                     dbManager,
                     SLOT(getUser(QString,QByteArray)));
    loginDialog->exec();
}

