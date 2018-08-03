/*
 * Ejemplo de uso de la librería RoJoArduino
 *
 * Para simplificar y normalizar la gestión de pines utilizaremos la librería
 * RoJoArduino para bmc2835. La misma utilizada para desarrollo de aplicaciones
 * sin interface gráfico con Geany.
 *
 * RoJoArduino está preparado para trabajar con C++ 2011.
 * Para activar esta opción en el compilador debemos incluir en el archivo .pro
 * la siguiente línea:
 *   CONFIG += c++11
 *
 * Además hay que copiar los archivos: RoJoArduino.cpp, bcm2835.h y bcm2835.c a
 * la carpeta de proyecto, e incluirlos en las secciones Headers y Sources.
 *
 * En el archivo que hace uso de la librería se debe añadir al principio:
 *   #include "RoJoArduino.cpp"
 * No es necesario referenciar a bcm3825 en ningún lugar. Ya se hace en RoJoArduino.cpp
 */

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "RoJoArduino.cpp"

#include <iostream>
using namespace std;

//Definición de pines
const int pinLed=21;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    //Si no se puede inicializar la librería...
    if(!arduinoOpen())
    {
        //...mostramos un error por la consola
        cout << "Error al inicializar gpio" << endl;
        //Terminamos sin inicializar los objetos gráficos
        //Aparecerá un formulario vacío
        return;
    }
    //Definimos el pin como salida
    pinMode(pinLed,OUTPUT);

    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    //Hemos terminado de utilizar la librería
    arduinoClose();

    delete ui;
}

void MainWindow::on_pushButton_toggle_clicked()
{
    //Pulsado el botón toggle

    //Variable con el estado del led
    //Queremos que se recuerde, por eso es estática
    static bool status = false;

    //Cambiamos el estado del led
    status=!status;

    //Leemos la imagen del archivo de recursos en función
    //del estado
    QPixmap image=QPixmap(status?":/icons/icons/bulb-on-48.png":":/icons/icons/bulb-off-48.png");
    //Asignamos la imagen al label
    ui->label_bulb->setPixmap(image);

    //Sección de gestión gpio
    digitalWrite(pinLed,status?HIGH:LOW);
}
