#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPainter>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

//Pulsado botón bombilla
void MainWindow::on_pushButton_Bombilla_clicked()
{
    //No se puede editar un QPixmap de un label puesto que se guarda como constante
    //Por lo tanto, necesitamos crear un QPixmap, que tras su edición, se asignará
    //al label, sustituyendo el actual.

    //Creamos un QPixmap nuevo con la misma imágen que tiene ahora
    //Obtenemos la imágen del archivo de recursos
    QPixmap myPixmap(":/images/icons/bulb-on-128.png");
    //Creamos un nuevo objeto QPainter para poder editarlo
    QPainter *paint = new QPainter(&myPixmap);
    //Asignamos color rosa al lápiz por defecto
    paint->setPen(QColor(255,34,255));
    //Dibujamos un rectángulo sobre la imágen actual
    paint->drawRect(15,15,100,100);
    //Borramos el objeto QPainter para que no haya pérdidas de memoria
    delete paint;
    //Tememos el QPixmap modificado
    //Lo asignamos al label
    ui->label->setPixmap(myPixmap);
}

//Pulsado el botón rejilla
void MainWindow::on_pushButton_Rejilla_clicked()
{
    //Creamos un QPixmap nuevo y vacío (negro)
    QPixmap myPixmap(201,201);
    //Creamos un nuevo objeto QPainter para poder editarlo
    QPainter *paint = new QPainter(&myPixmap);
    //Lo pintamos de blanco con un rectángulo relleno
    paint->fillRect(0,0,200,200,QColor(255,255,255));
    //Asignamos color negro al lápiz por defecto
    paint->setPen(QColor(0,0,0)); //Negro
    //Dibujamos una rejilla
    for(int i=0;i<=200;i+=10)
    {
        paint->drawLine(0,i,200,i);
        paint->drawLine(i,0,i,200);
    }
    //Borramos el objeto QPainter para que no haya pérdidas de memoria
    delete paint;
    //Tememos el QPixmap modificado
    //Lo asignamos al label
    ui->label->setPixmap(myPixmap);
}
