#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //Incluimos algunos widgets en la barra de estado
    //Metemos la barra de progreso en la bara de estado
    ui->statusBar->addPermanentWidget(ui->progressBar,2);
    //Metemos el label en la barra de estado
    ui->statusBar->addPermanentWidget(ui->label,1);
}

MainWindow::~MainWindow()
{
    delete ui;
}
