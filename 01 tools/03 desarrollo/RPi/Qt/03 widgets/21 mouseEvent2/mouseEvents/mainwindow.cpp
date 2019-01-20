#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->label,SIGNAL(mousePressSignal(QPoint&,int&)),this,SLOT(mousePressSlot(QPoint&,int&)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::mousePressSlot(QPoint &xy,int &button)
{
    ui->lcdNumberX->display(xy.x());
    ui->lcdNumberY->display(xy.y());
    ui->lcdNumberButton->display(button);
}
