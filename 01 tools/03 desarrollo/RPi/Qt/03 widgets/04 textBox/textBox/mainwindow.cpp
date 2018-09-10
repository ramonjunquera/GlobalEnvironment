/* Objetivo:
 * Aprender a utilizar el objeto plainTextEdit
 *
 * Descripción:
 * El objeto plainTextEdit es un textEdit que sólo puede contener
 * texto plano. Es mucho más ligero.
 * Pensado para grandes cantidades de texto, porque no tiene que
 * formatearlo como lo hace el objeto textEdit.
 *
 * El formulario principal tiene 3 plainTextEdit
 * Source: En el que se escribe
 * Clone: En el que se copia el contenido de Source a cada nueva
 *   pulsación. Es de sólo lectura.
 * Destination: En el que se copia el contenido de Source al
 *   pulsar el botón.
 *
 * Con el botón aprendemos a obtener el texto de un plainTextEdit y
 * a asignarlo.
 * Tenemos el primer contacto con los strings de QT.
 * Aprendemos el uso del evento textChanged de un plainTextEdit cuando
 * copiamos a Clone.
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

void MainWindow::on_pushButton_Copy_clicked()
{
    //Se ha pulsado el botón Copy
    //Copiamos el texto de Source a Destination

    //Tomamos nota del texto del plainTextBox_Source
    //En QT todas las variables String han sido sustituidas por
    //una variación propia llamada QString
    QString t=ui->plainTextEdit_Source->toPlainText();
    //Escribimos el texto en plainTextBox_Destination
    ui->plainTextEdit_Destination->document()->setPlainText(t);
}

void MainWindow::on_plainTextEdit_Source_textChanged()
{
    //Ha cambiado el texto de Source

    //Estamos utilizando el evento textChanged

    //Lo copiamos automáticamente a Clone
    QString t=ui->plainTextEdit_Source->toPlainText();
    ui->plainTextEdit_Clone->document()->setPlainText(t);
}
