/*
 * Ejemplo de botón checkable
 *
 * Al igual que los checkBox, un botón puede dejarse pulsado.
 * Para ello debemos activar la opción checkable de la sección QAbstractButton
 *
 * El ejemplo muestra como en cada pulsación del botón Change, se cambia el estado
 * del botón Checkable con el metodo Toggle().
 * A su vez, cada vez que el boon Checkable cambia de estado, se lo pasa al
 * checkBox.
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

void MainWindow::on_pushButton_Change_clicked()
{
    //Pulsado el botón Change

    //Cambiamos el estado del botón Checkable
    ui->pushButton_Checkable->toggle();
}

void MainWindow::on_pushButton_Checkable_toggled(bool checked)
{
    //Se ha cambiado el estado del botón Checkable

    //Asignamos el mismo estado al checkBox_Status
    ui->checkBox_Status->setChecked(checked);
}
