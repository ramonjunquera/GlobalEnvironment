#include "mainwindow.h"
#include "ui_mainwindow.h"

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
    //Pulsado el botón de crear ventana
    DialogWindow *d=new DialogWindow(this);
    //Creamos la variable en la que se devolverá la respuesta
    QString answer;
    //Si se ha aceptado...aplicamos la respuesta en el label
    if(d->open(&answer)) ui->label->setText(answer);
    //Borramos la instancia de memoria
    delete(d);
}
