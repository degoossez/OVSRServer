#include "server.h"


/*! \brief Constructor
 *
 * @param w A pointer used to acces the widgets on the main window
 * @param parent A pointer to the parent object
 */
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
    if(!server->listen(QHostAddress::Any,PORT))
    {
        emit drawLabel("<ERROR> Server is unable to listen on port " + QString::number(PORT) + "!\n");
    }
    else
    {
        emit drawLabel("<INFO> Server started on port " + QString::number(PORT) + ".\n <INFO> Waiting for client.");
    }

    connect(server,SIGNAL(newConnection()),this,SLOT(CreateTcp()));
    connect(this,SIGNAL(dataReceived(QByteArray)),w,SLOT(changeUI(QByteArray)));

    process = new QProcess();
    connect(process,SIGNAL(readyReadStandardOutput()),this,SLOT(readStdIn()));
    connect(process,SIGNAL(readyReadStandardError()),this,SLOT(readStdError()));

    dbManager->openDB();

}

/*! \brief Initialisaton for TCP communication
 *
 * Creates the neccessary objects and signal slot connections for TCP communication.
 * When a user connection is made to the server, a notification is shown on the main window.
 * @return the last SQL error
 */

void Server::CreateTcp()
{
    //MainWindow w;
    tcpSocket = new QTcpSocket;
    tcpSocket = server->nextPendingConnection();
    connect(tcpSocket,SIGNAL(readyRead()),this,SLOT(ReadTcp()));
    qDebug("TCP connection made with a client");
    emit drawLabel("<INFO> TCP connection made with a client");
}

/*! \brief Read incoming TCP packets
 *
 * Read incoming data from the TCP socket. This function will act depending on the received data:
 *
 * STARTPACKAGE: indicates the start of a package containing user credentials, API level and Renderscript
 * code. The username with hashed password will be searched in the SQL database. When this information is
 * incorrect a login error is send to the client.
 *
 * ENDPACKAGE: if the user is correctly authenticated, the Renderscript code, which is completely received(indicated by the
 * endpackage), will be compiled to bytecode.
 *
 * LOGIN: a user login request is received.Search the user in the SQL database. Respond to the client accordingly with
 * login ok or login error.
 *
 * ACCOUNT: the user wants to create an account. First check if username is already in use. If not, create a new entry in the
 * database. Next the user is created in the FTP user list.
 *
 *
 */
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
                process->start("mkdir "FTP_USERS_DIR + username);
                process->waitForFinished();
                process->start("pure-pw useradd " + username + "  -u "FTP_GROUP" -d "FTP_USERS_DIR + username);
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
                qDebug() << "rsCode = " + rsCode;
                qDebug("End received");
                readRS=false;

                if(rsCode == "")
                {
                    rsCode = Datacp.replace(QByteArray("ENDPACKAGE"), QByteArray(""));
                    qDebug() << "empty Code : " + rsCode;
                }

                if(isValid)
                {
                    QFile file(FTP_USERS_DIR + username + "/template.rs");
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

/*! \brief send TCP message
 *
 * @param data The message
 */
void Server::WriteTcp(QByteArray data)
{
    tcpSocket->write(data);
    tcpSocket->flush();
}

/*! \brief Reads the standard input from QProcess
 *
 * This function reads the standard input from a QProcess, and act accordingly.
 *
 * When QProcess has been used for compiling the Renderscript Code (if the variable compiling is true)
 * we know that there were no compile errors and a TCP message will be send to indicate this.
 *
 * When QProcess has been used to create a FTP user, the first time this function is called the password is written
 * to the Process. The second time this action will be repeated to confirm the password.
 *
 */
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

/*! \brief Reads the standard error from QProcess
 *
 * This function reads the standard error from a QProcess.
 * The error message, which contains the compiler errors, is send via TCP to the client
 *
 */
void Server::readStdError()
{
    QByteArray errors =  process->readAllStandardError();
    WriteTcp(errors + "\n");
    qDebug()<< "Error message!" + errors;
}

/*! \brief Compiles the RenderScript code
 *
 * Calls the RenderScript compiler, the output folder depends on the username of the client.
 *
 * @param apiLevel the API level is used to build the right bytecode that can run on the client's device
 */
void Server::compileRS(QByteArray apiLevel)
{
        qDebug() << "Compiling";
        qDebug() << SDK_PATH"/build-tools/"BUILD_TOOLS_VERSION"/llvm-rs-cc -target-api " + apiLevel + " -o "FTP_USERS_DIR + username + " -p "FTP_USERS_DIR + username + " -I "SDK_PATH"/build-tools/"BUILD_TOOLS_VERSION"/renderscript/include -I "SDK_PATH"/build-tools/"BUILD_TOOLS_VERSION"/renderscript/clang-include "FTP_USERS_DIR + username + "/template.rs";
        process->start(SDK_PATH"/build-tools/"BUILD_TOOLS_VERSION"/llvm-rs-cc -target-api " + apiLevel + " -o "FTP_USERS_DIR + username + " -p "FTP_USERS_DIR + username + " -I "SDK_PATH"/build-tools/"BUILD_TOOLS_VERSION"/renderscript/include -I "SDK_PATH"/build-tools/"BUILD_TOOLS_VERSION"/renderscript/clang-include "FTP_USERS_DIR + username + "/template.rs");

        qDebug() << "Compiling done";
}

/*! \brief Shows a dialog to create a user directly on the server
 *
 **/

void Server::createUserDialog()
{
    LoginDialog* loginDialog = new LoginDialog();
    connect( loginDialog,
                     SIGNAL(acceptLogin(QString,QByteArray)),
                     dbManager,
                     SLOT(insertUser(QString,QByteArray)));
    loginDialog->exec();

}

/*! \brief Shows a dialog to search for a user
 *
 **/
void Server::searchUserDialog() {
    LoginDialog* loginDialog = new LoginDialog();
    connect( loginDialog,
                     SIGNAL(acceptLogin(QString,QByteArray)),
                     dbManager,
                     SLOT(getUser(QString,QByteArray)));
    loginDialog->exec();
}

