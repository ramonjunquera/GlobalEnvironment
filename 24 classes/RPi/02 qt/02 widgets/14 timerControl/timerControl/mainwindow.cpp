#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    //Constructor de la clase del formulario

    //Creamos una nueva instancia del timer
    myTimer = new QTimer(this);
    //Ahora ya podemos inicializar los objetos gráficos
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    //Destructor de la clase del formulario

    delete ui;
}

void MainWindow::BulbChangeStatus()
{
    //Cambia el estado de la bombilla

    //Definimos variable que guarda el estdo de la bombilla
    static bool bulbStatus=false;

    //Cambiamos el estado de la bombilla
    bulbStatus=!bulbStatus;
    //Cargamos la imagen teniendo el cuenta el estado
    QPixmap image(bulbStatus?":/icons/icons/bulb-on-48.png":":/icons/icons/bulb-off-48.png");
    //Asignamos la imagen al label
    ui->label_bulb->setPixmap(image);
}

void MainWindow::TimerLauncherOnce()
{
    //Función a la que llama el timer de una sola vez

    //Cambiamos el estado de la bombilla
    BulbChangeStatus();
    //Activamos el botón Once
    ui->pushButton_Once->setEnabled(true);
    //Activamos el botón Start
    ui->pushButton_Start->setEnabled(true);
}

void MainWindow::on_pushButton_Start_clicked()
{
    //Pulsado botón Start

    //Desactivamos el botón Start
    ui->pushButton_Start->setEnabled(false);
    //Desactivamos el botón Once
    ui->pushButton_Once->setEnabled(false);

    //Le asignamos la función a la que llamar
    connect(myTimer,SIGNAL(timeout()),this,SLOT(BulbChangeStatus()));
    //Arrancamos el timer para que se ejecute cada segundo
    myTimer->start(1000);

    //Activamos el botón End
    ui->pushButton_End->setEnabled(true);

}

void MainWindow::on_pushButton_End_clicked()
{
    //Pulsado botón End

    //Desactivamos el botón End
    ui->pushButton_End->setEnabled(false);
    //Activamos el botón Start
    ui->pushButton_Start->setEnabled(true);

    //Paramos el timer
    myTimer->stop();

    //Activamos el botón Once
    ui->pushButton_Once->setEnabled(true);

}

void MainWindow::on_pushButton_Once_clicked()
{
    //Pulsado botón Once

    //Desactivamos el botón Once
    ui->pushButton_Once->setEnabled(false);
    //Desactivamos el botón Start
    ui->pushButton_Start->setEnabled(false);

    //Arrancamos el timer para que se ejecute sólo una vez dentro de un segundo
    myTimer->singleShot(1000,this,SLOT(TimerLauncherOnce()));
}
