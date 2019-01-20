#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->label,SIGNAL(mousePressSignal(QPoint&,int&)),this,SLOT(mousePressSlot(QPoint&,int&)));
    connect(ui->label,SIGNAL(mouseReleaseSignal(QPoint&)),this,SLOT(mouseReleaseSlot(QPoint&)));
    connect(ui->label,SIGNAL(mouseMoveSignal(QPoint&)),this,SLOT(mouseMoveSlot(QPoint&)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::mousePressSlot(QPoint &xy,int &button)
{
    //Se ha recibido una signal de que se ha pulsado el botón del mouse

    //Mostramos coordenadas
    ui->lcdNumberClickX->display(xy.x());
    ui->lcdNumberClickY->display(xy.y());
    //Mostramos botón pulsado
    ui->lcdNumberClickButton->display(button);
}

void MainWindow::mouseReleaseSlot(QPoint &xy)
{
    //Se ha recibido una signal de que se ha soltado el botón del mouse

    //No tenemos en cuenta las coordenadas del punto donde se ha soltado el botón
    //Sólo reflejaremos que el botón pulsado es ninguno = 0
    ui->lcdNumberClickButton->display(0);
}

void MainWindow::mouseMoveSlot(QPoint &xy)
{
    //Se ha recibido una signal de que se ha movido el mouse

    //Mostramos las coordenadas
    ui->lcdNumberMoveX->display(xy.x());
    ui->lcdNumberMoveY->display(xy.y());
}
