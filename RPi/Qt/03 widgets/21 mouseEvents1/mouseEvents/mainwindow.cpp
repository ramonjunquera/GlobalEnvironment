#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->label,SIGNAL(mousePressSignal(QPoint&)),this,SLOT(mousePressSlot(QPoint&)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::mousePressSlot(QPoint &xy)
{
    ui->lcdNumberX->display(xy.x());
    ui->lcdNumberY->display(xy.y());
}
