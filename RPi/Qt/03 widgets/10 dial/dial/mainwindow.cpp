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

void MainWindow::on_dial_2_valueChanged(int value)
{
    //Definimos la variable donde guardaremos el valor
    QString s;
    //Aprovechamos que ya nos pasan el valor del dial como parámetro y
    //lo convertimos s QString
    s.setNum(value);
    //Asignamos QString al objeto de texto plano
    ui->plainTextEdit->document()->setPlainText(s);
}

void MainWindow::on_dial_3_valueChanged(int value)
{
    //Definimos la variable donde guardaremos el valor
    QString s;
    //Aprovechamos que ya nos pasan el valor del dial como parámetro y
    //lo convertimos s QString
    s.setNum(value);
    //Asignamos el QString a objeto de línea de texto
    ui->lineEdit->setText(s);
}
