#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow)
{
  ui->setupUi(this);
  //Conectamos el evento toggled del objeto pushButton a la función lambda
  //Se le pasa el mismo parámetro de toggled
  connect(ui->pushButton,&QPushButton::toggled,[&](bool checked){
    ui->label->setText(checked==true?"pulsado":"soltado");
  });
}

MainWindow::~MainWindow()
{
  delete ui;
}
