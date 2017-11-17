/*
 * Autor: Ramón Junquera
 * Tema: Librería RoJoArduino
 * Objetivo: Escritura de pines digitales
 * Material: Raspberry Pi, breadboard, cables, led
 * Descripción:
 * Crearemos un formulario con un botón y una imagen.
 * El botón cambiará el estado del pin 21 en el que tenemos conectado
 * un led.
 * La imagen representa el estado del led real.
 *
 * Utilizamos por primera vez la librería Arduino.h
 * Para no duplicar librerías en la programación en Geany acordamos
 * guardar todas las librerías que podemos utilizar en la carpeta
 * prg de la home del usuario pi: /home/pi/prg
 * La librería más importante se llama Arduino.h que no tiene
 * dependencias y que se encarga de facilitar las funciones del IDE de
 * Arduino en Raspberry.
 * Por lo tanto, incluiremos su referencia en al inicio de la definición
 * del formulario principal (mainwindow.cpp):
 *   #include "/home/pi/prg/Arduino.h"
 *
 * Para cumplir una programación formal, incluimos en el destructor del
 * formulario principal el comando de finalización de las funciones de
 * gestión de pines: ArduinoEnd()
 *
 * Esta librería utiliza las ventajas de la versión 11 de c++
 * Para que la reconozca sin problemas, debemos añadir al archivo del
 * proyecto (.pro) la siguiente línea:
 *   CONFIG += c++11
 * Esto le indicará al compilador que incluya las funcionalidades de esta versión.
 *
 * El estado del led se guarda en una variable en la misma función de
 * pulsación del botón.
 * Para que mantenga el valor correcto la hacemos estática.
 *
 * Para la imágen se utilizan los conceptos de:
 * - Etiqueta con gráficos
 * - Archivo de recursos
 */

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "/home/pi/prg/Arduino.h"

//Definición de pines
const int pinLed=21;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    //Constructor del formulario

    //Definimos el pin como salida
    pinMode(pinLed,OUTPUT);

    //Ejecución normal
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    //Destructor del formulario

    ArduinoEnd(); //Terminamos de usar la librería

    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    //Se ha pulsado el botón

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
