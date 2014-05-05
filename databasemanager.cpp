#include "databasemanager.h"
#include <QDir>


DatabaseManager::DatabaseManager(QObject *parent) :
    QObject(parent)
{
}
DatabaseManager::~DatabaseManager()
{

}

bool DatabaseManager::openDB()
{
    qDebug() << "inside openDB";

    // Find QSLite driver
    db = QSqlDatabase::addDatabase("QSQLITE");

    #ifdef Q_OS_LINUX
    // NOTE: We have to store database file into user home folder in Linux
    QString path(QDir::home().path());
    path.append(QDir::separator()).append("my.db.sqlite");
    path = QDir::toNativeSeparators(path);
    db.setDatabaseName(path);
    qDebug() << "Pad: " << path;
    #else
    // NOTE: File exists in the application private folder, in Symbian Qt implementation
    db.setDatabaseName("my.db.sqlite");
    #endif

    // Open databasee
    if(!db.isOpen())
        return db.open();
    else
        return false;
}

QSqlError DatabaseManager::lastError()
{
    // If opening database has failed user can ask
    // error description by QSqlError::text()
    return db.lastError();
}

bool DatabaseManager::deleteDB()
{
    qDebug() << "inside deleteDB";

    // Close database
    db.close();

    #ifdef Q_OS_LINUX
    // NOTE: We have to store database file into user home folder in Linux
    QString path(QDir::home().path());
    path.append(QDir::separator()).append("my.db.sqlite");
    path = QDir::toNativeSeparators(path);
    return QFile::remove(path);
    #else

    // Remove created database binary file
    return QFile::remove("my.db.sqlite");
    #endif
}

bool DatabaseManager::createUsersTable()
{
    qDebug() << "inside createUsersTable";

    // Create table "person"
    bool ret = false;
    if (db.isOpen())
        {
        QSqlQuery query;
        ret = query.exec("create table user "
                  "(id integer primary key, "
                  "username varchar(40), "
                  "password varchar(40))");

        }
    if(ret == true)
        qDebug() << "table created";
    else
        qDebug() << "table not created";
    return ret;
}

int DatabaseManager::insertUser(QString username, QByteArray password)
{
    int newId = -1;
    bool ret = false;

    qDebug() << "inside insertUser " << "username = " + username + " password = " + password;

//    QByteArray hash = QCryptographicHash::hash(password,QCryptographicHash::Md5);
//    qDebug() << "hash = " + hash + " " + hash.toHex();

    QString hash = QString(QCryptographicHash::hash((password),QCryptographicHash::Md5).toHex());
    qDebug() << "hash = " + hash;


    if (db.isOpen())
    {
        // NULL = is the keyword for the autoincrement to generate next value

        QSqlQuery query;
        ret = query.exec(QString("insert into user values(NULL,'%1','%2')")
        .arg(username).arg(hash));

           // Get database given autoincrement value
        if (ret)
        {
            newId = query.lastInsertId().toInt();
            qDebug() << "user created with ID" + QString::number(newId);
        }
        else
            qDebug() << "error cannot create user : " + query.lastError().text();


       }


    return newId;
}

bool DatabaseManager::getUser(QString username, QByteArray password){
    bool ret = false;

    //qDebug() << "inside getUser : username = " + username + " passwd = " + password;

    QSqlQuery query(QString("select * from user where username = '%1'").arg(username));
    if (query.next())
    {
        qDebug() << "user " + username + " found";

        //QString hash = QString(QCryptographicHash::hash((password),QCryptographicHash::Md5).toHex());

        if(QString(password) == query.value(2).toString())
        {
            qDebug() << "password OK";
            ret = true;
        }
        else
            qDebug() << "password error";


    }
    else
        qDebug() << username + " : not found";

    return ret;
}

void DatabaseManager::printDB() {

    QSqlQuery query(QString("select * from user"));
    while(query.next())
    {
        qDebug() << query.value(0).toString() + " " + query.value(1).toString() + " " + query.value(2).toByteArray();

    }

}


