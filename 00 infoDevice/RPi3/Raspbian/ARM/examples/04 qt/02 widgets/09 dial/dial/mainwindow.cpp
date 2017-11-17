/*
 * Ejemplo de uso de objetos dial y lineEdit
 *
 * dial permite la selección de un número entero dentro de un rango.
 * Aprovechamos el evento valueChanged para actualizar un indicador numérico.
 *
 * lineEdit  es similar a plaintTextEdit, pero permite alinear el texto.
 *
 * El ejemplo contiene dos pares de objetos enlazados.
 * El superior con lineEdit se enlazan por código.
 * El inferior con lcdNumber se enlazan por signals/slots
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

void MainWindow::on_dial_1_valueChanged(int value)
{
    //Ha cambiado el valor de dial_1

    //Lo convertimos en QString
    QString s;
    s.setNum(value);
    //Asignamos el texto a lineEdit
    ui->lineEdit->setText(s);
}
