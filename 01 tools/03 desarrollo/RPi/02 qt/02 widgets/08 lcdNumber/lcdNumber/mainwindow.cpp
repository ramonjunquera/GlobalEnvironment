/*
 * Ejemplo de uso de objeto lcdNumber
 *
 *
 */

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

void MainWindow::on_pushButton_DEC_clicked()
{
    //Pulsado el botón de decrementar

    //Como ejemplo, obtenemos el valor actual del objeto lcdNumber
    //y lo decrementamos en una unidad
    int value = ui->lcdNumber->intValue()-1;
    //Asignamos el valor calculado al objeto lcdNumber
    ui->lcdNumber->display(value);
}

void MainWindow::on_pushButton_INC_clicked()
{
    //Pulsado el botón de incrementar

    //En una sóla línea obtenemos el valor del objeto lcdNumber,
    //incrementamos su valor en una unidad y lo volvemos a
    //asignar al mismo objeto
    ui->lcdNumber->display(ui->lcdNumber->intValue()+1);
}
