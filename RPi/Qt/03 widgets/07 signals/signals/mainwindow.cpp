/*
 * Ejemplo de signals/slots
 *
 * QT permite automatizar el traspaso de información entre componentes gráficos.
 * Cuando se genera un evento, puede lanza un método en otro.
 * Los tipos deben coincidir.
 * Si el evento no tiene parámetros, sólo podrá llamar a métodos sin parámetros.
 * Si genera un parámetro bool, podrá llamar tamién a métodos con un parámetro bool.
 * Se pueden crear varios enlaces entre objetos. Incluso en cascada.
 *
 * Para gestionar los signals debemos estar en modo de diseño y pulsar el botón
 * "Edit Signals/Slots F4". Es el segundo botón de la barra.
 * Para volver al modo normal de edición, pulsamos el botón "Edit Widgets F3".
 * Primer botón de la barra.
 *
 * En este caso tenemos 3 objetos.
 * El botón de Step1 cuando es pulsado cambia el estado del checkBox_Step2.
 * Cuando el checkBox_Step2 cambia de estado, lo copia en el checkBox_Step3.
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
