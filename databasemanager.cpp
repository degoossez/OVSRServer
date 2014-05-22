#include "databasemanager.h"
#include <QDir>

 /*! \brief Constructor
  *
  * @param parent A pointer to the parent object
  */
DatabaseManager::DatabaseManager(QObject *parent) :
    QObject(parent)
{
}

/*! \brief Destructor

 */
DatabaseManager::~DatabaseManager()
{

}

/*! \brief Opens the SQL database
 *
 * Opens the SQL database located in the user's home directory when working in Linux
 * and in the application's private folder when working on Windows.
 *
 * @return true when database is succesfully opened, otherwise returns false
 */

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
/*! \brief returns the last SQL error

 *
 * @return the last SQL error
 */

QSqlError DatabaseManager::lastError()
{
    // If opening database has failed user can ask
    // error description by QSqlError::text()
    return db.lastError();
}

/*! \brief Deletes the SQL database
 *
 * Deletes the SQL database located in the user's home directory when working in Linux
 * and in the application's private folder when working on Windows.
 *
 * @return true when database is succesfully deleted, otherwise returns false.
 */

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

/*! \brief Creates a table for user data storage
 *
 * Creates a table in the currently opened database for the storage of user Data.
 * The table has 3 fields: ID username password.
 * This can fail when no database is opened or if the table already exists.
 *
 * @return true when table is succesfully created, otherwise returns false.
 */

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

/*! \brief inserts a new user into the dabase
 *
 * Inserts a new user into the database. This can fail when no database is opened or when
 * the user table is not present.
 * @param username name of the user.
 * @param password password of the user.
 *
 * @return ID this is the ID of the new entry.
 */

int DatabaseManager::insertUser(QString username, QByteArray password)
{
    int newId = -1;
    bool ret = false;

    qDebug() << "inside insertUser " << "username = " + username + " password = " + QString(password);

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

/*! \brief Search a user in the database.
 *
 * Looks up a user in the database. Both username and password must be found.
 * A database must be opened.
 * @param username name of the user.
 * @param password password of the user.
 *
 * @return true if user is found, otherwise false.
 */

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

/*! \brief Search a user in the database.
 *
 * Looks up a user in the database. Only the username is used to locate the user.
 * A database must be opened.
 * @param username name of the user.
 *
 * @return true if user is found, otherwise false.
 */
bool DatabaseManager::getUserName(QString username)
{
    bool ret = false;

    QSqlQuery query(QString("select * from user where username = '%1'").arg(username));
    if (query.next())
    {
        ret = true;
    }

    return ret;

}

/*! \brief Prints the content of the user table in the database
 *
 * All entries in the user table of the opened database will be displayed in the debug window.
 * This can fail when no database is opened or present. The user table must exist with at least one entry.
 *
 */

void DatabaseManager::printDB() {

    QSqlQuery query(QString("select * from user"));
    while(query.next())
    {
        qDebug() << query.value(0).toString() + " " + query.value(1).toString() + " " + query.value(2).toByteArray();

    }

}


