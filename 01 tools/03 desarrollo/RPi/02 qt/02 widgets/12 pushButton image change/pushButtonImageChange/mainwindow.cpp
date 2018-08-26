/*
 * Ejemplo de cambio de icono a un botón
 *
 * El formulario tiene dos botones.
 * El primero con un icono y sin funcionalidad en su pulsación.
 * El segundo cambia el icono del primero.
 *
 * Guardamos las imágenes de los iconos en un archivo de recursos.
 *
 * Aprovechamos para eliminar los elementos por defecto que se
 * crean con un proyecto:
 *   statusBar
 *   mainToolBar
 *   mainBar
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

void MainWindow::on_pushButton_toggle_clicked()
{
    //Pulsado el botón toggle

    //Variable con el estado. Estática para que se recuerde
    static bool status=false;

    //Cambiamos el estado
    status=!status;

    //Leemos la imagen del archivo de recursos dependiendo del estado
    QPixmap myImage=QPixmap(status?":/images/icons/bulb-on-48.png":":/images/icons/bulb-off-48.png");
    //La convertimos en un icono
    QIcon myIcon=QIcon(myImage);
    //La asignamos al botón
    ui->pushButton_bulb1->setIcon(myIcon);
    //No tenemos que preocuparnos por el tamaño del botón, puesto que todas la imágenes
    //tienen las mismas dimensiones
}
