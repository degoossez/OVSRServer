#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "qapplication.h"
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    VLayout = new QVBoxLayout(ui->centralWidget);
    Info = new QLabel();
    ui->centralWidget->setLayout(VLayout);
    VLayout->addWidget(Info);
    Info->setText("Test");
    openDB = new QPushButton("Open Database");
    VLayout->addWidget(openDB);
    deleteDB = new QPushButton("Delete Database");
    VLayout->addWidget(deleteDB);
    createTabel = new QPushButton("Create Tabel");
    VLayout->addWidget(createTabel);
    createUser = new QPushButton("Create user");
    VLayout->addWidget(createUser);
    searchUser = new QPushButton("Search User");
    VLayout->addWidget(searchUser);
    showDatabase = new QPushButton("Show database");
    VLayout->addWidget(showDatabase);
}

MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::changeUI(QByteArray DataTcp)
{

}
void MainWindow::drawLabel(QString Text)
{
    Info->setText(Text);
    Info->update();
    Info->repaint();
}
