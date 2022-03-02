#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    //Se ha pulsado el botón de selección de imágen

    //Pedimos que se seleccione un único archivo
    QString fileName = QFileDialog::getOpenFileName(this,tr("Selecciona archivo"),"",tr("Imágenes (*.png *.jpg *.bmp);;Todos los archivos (*.*)"));
    //Si no se ha cancelado...
    if(!fileName.isNull())
    {
        //Creamos un nuevo pixmap
        QPixmap myPixmap;
        //Si hemos podido cargar la imagen...la asignamos al label
        if(myPixmap.load(fileName)) ui->label->setPixmap(myPixmap);
    }
}
