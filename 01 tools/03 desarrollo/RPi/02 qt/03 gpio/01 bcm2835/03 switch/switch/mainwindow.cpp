#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "RoJoArduino.cpp"

#include <iostream>
using namespace std;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    //Constructor de la clase del formulario

    //Si no se puede inicializar la librerí­a...
    if(!arduinoOpen())
    {
        //...mostramos un error por la consola
        cout << "Error al inicializar gpio" << endl;
        //Terminamos sin inicializar los objetos gráficos
        //Aparecerá un formulario vací­o
        return;
    }
    //Hemos podido inicializar la librería

    //Definimos el pin como entrada con resistencias de pullup
    pinMode(pinSwitch,INPUT_PULLUP);

    //Creamos una nueva instancia del timer
    myTimer = new QTimer(this);
    //Le asignamos la función a la que llamaár
    connect(myTimer,SIGNAL(timeout()),this,SLOT(TimerLauncher()));
    //Arrancamos el timer para que se ejecute cada décima de segundo
    myTimer->start(100);
    //Ahora ya podemos inicializar los objetos gráficos
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    //Paramos el timer
    myTimer->stop();
    //Destruimos el timer
    delete myTimer;
    //Hemos terminado de utilizar la librería
    arduinoClose();
    //Destruimos el objeto de interface de usuario
    delete ui;
}

void MainWindow::TimerLauncher()
{
    //Función a la que llama el timer

    //Leemos el estado actual del interruptor
    //Cambiamos los estados, porque al tener resistencias de pullup, si no
    //se pulsa dará HIGH
    bool currentSwitchStatus=(digitalRead(pinSwitch)>0?false:true);
    //Si tiene el mismo estado que el pushButton...no hacemos nada
    if(currentSwitchStatus==ui->pushButton_status->isChecked()) return;
    //Ha cambiado el estado desde la vez anterior!
    //Aplicamos el nuevo estado
    ui->pushButton_status->setChecked(currentSwitchStatus);
    //Si el interruptor está pulsado ahora...
    if(currentSwitchStatus)
    {
        //...es porque se acaba de pulsar.
        //Cambiaremos el estado de la bombilla
        ChangeBulbStatus();
    }
}

void MainWindow::ChangeBulbStatus()
{
    //Cambia el estado de la bombilla

    //Estado de la bombilla
    static bool bulbStatus=false;

    //Cambiamos el estado del led
    bulbStatus=!bulbStatus;
    //Leemos la imagen del archivo de recursos en función del estado
    QPixmap image=QPixmap(bulbStatus?":/icons/icons/bulb-on-48.png":":/icons/icons/bulb-off-48.png");
    //Asignamos la imagen al label
    ui->label_bulb->setPixmap(image);
}
