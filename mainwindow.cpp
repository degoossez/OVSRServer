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
