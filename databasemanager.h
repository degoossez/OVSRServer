#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlError>
#include <QFile>
#include <QSqlQuery>
#include <QDebug>
#include <QCryptographicHash>


class DatabaseManager : public QObject
{
    Q_OBJECT
public:
        DatabaseManager(QObject *parent = 0);
        ~DatabaseManager();

    public slots:
        bool openDB();
        bool deleteDB();
        QSqlError lastError();
        bool createUsersTable();
        int insertUser(QString , QByteArray);
        bool getUser(QString, QByteArray);
        void printDB();

    private:
        QSqlDatabase db;
    
};

#endif // DATABASEMANAGER_H
