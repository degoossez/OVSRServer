#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QTextEdit>
#include <QLabel>
#include <QDebug>
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    QVBoxLayout *VLayout;
    QLabel *Info;
    QPushButton *openDB;
    QPushButton *deleteDB;
    QPushButton *createTabel;
    QPushButton *createUser;
    QPushButton *searchUser;
    QPushButton *showDatabase;

public slots:
    void changeUI(QByteArray );
    void drawLabel(QString );
private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
