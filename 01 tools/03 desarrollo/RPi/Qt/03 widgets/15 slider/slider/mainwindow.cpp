/*
 * Ejemplo de uso de slider
 *
 * Utilizamos dos sliders. Uno horizontal y otro
 * vertical, que están asociados a distintos
 * lcdNumber.
 *
 * En los objetos lcdNumber se ha reducido el número
 * de dígitos a mostrar a 2 para que no ocupen tanto.
 * Además los sliders están limitados a [0,99].
 * Nunca tendrán más de dos dígitos.
 *
 * No se ha creado un slot para relacionar el slider
 * con su lcdNumber porque se ha hecho con
 * Signals/Slots.
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
