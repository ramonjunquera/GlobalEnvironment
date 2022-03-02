#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->label,SIGNAL(mousePressSignal(QPoint&,int&)),this,SLOT(mousePressSlot(QPoint&,int&)));
    connect(ui->label,SIGNAL(mouseReleaseSignal(QPoint&)),this,SLOT(mouseReleaseSlot(QPoint&)));
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

void MainWindow::mouseReleaseSlot(QPoint &xy)
{
    //No tenemos en cuenta las coordenadas del punto donde se ha soltado el botón
    //Sólo reflejaremos que el botón pulsado es ninguno = 0
    ui->lcdNumberButton->display(0);
}
