/*
 * Ejemplo de uso de librería bcm2835 con pines digitales
 *
 * Consistirá en un botón que cambia el estado de un led y además lo muestra
 * con una imagen.
 *
 * Sólamente hay que dejar en la carpeta del proyecto los archivos:
 *   bcm2835.c
 *   bcm2835.h
 * No es obligatorios incluirlos en las secciones de Headers y Sources, pero si se
 * hace, como es este ejemplo, se reconocerán las funciones incluidas y será más
 * fácil utilizarlas.
 *
 * Además hay que añadir la línea
 *   #include "bcm2835.c"
 * al principio del código que la vaya a utilizar. En este caso mainwindow.cpp
 *
 * Los archivos bcm2835.* se han tomado de la instalación original de la librería.
 * Concretamente se encontraban en /home/pi/bcm2835-1.52/src
 * El archivo bcm2835.c ha sido modificado para que conincidan los tipos que daban
 * error al compilarlo.
 *
 * Existe una limitación en el uso de esta librería. No puede ser llamada más de una
 * vez. Por lo tanto la usamos sólo desde la clase del formulario.
 * Inicializamos la gestión de pines en el constructor y terminamos en el destructor.
 * Si no se puede inicializar la librería en el constructor, enviamos un mensaje de
 * error a la consola y no inicializamos el User Interface (ui). Por lo tanto
 * obtendremos un formulario sin objetos gráficos (vacío).
 */

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "bcm2835.c"

#include <iostream>
using namespace std;

//Definición de pines
const int pinLed=21;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    //Si no se puede inicializar la librería...terminamos
    if(!bcm2835_init())
    {
        //Mostramos un error por la consola
        cout << "Error al inicializar gpio" << endl;
        //Terminamos sin inicializar los objetos gráficos
        //Aparecerá un formulario vacío
        return;
    }
    //Definimos el pin como salida
    bcm2835_gpio_fsel(pinLed,BCM2835_GPIO_FSEL_OUTP);

    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    //Hemos terminado de utilizar la librería
    bcm2835_close();

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
    bcm2835_gpio_write(pinLed,status?HIGH:LOW);
}
