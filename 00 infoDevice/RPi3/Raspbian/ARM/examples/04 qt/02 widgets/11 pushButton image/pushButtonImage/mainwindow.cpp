/*
 * Ejemplo de botón con imágen
 *
 * Creamos un archivo de recursos y deé l seleccionamos la imagen que pondremos como
 * icono.
 * Es importante que tras crear el archivo de recursos guardemos todo el proyecto para
 * que quede reflejado y se pueda referenciar desde cualquier sitio.
 *
 * Para signar una imagen a un pushButton, seleccionamos la propiedad
 *   QAbstractButton/icon/Normal Off
 * Este es el icono de un botón no pulsado.
 * Se puede cargar una imagen distinta para cada uno de sus estados.
 *
 * Todas la imágenes cargadas son reducidas por defecto a un tamaño de 16x16.
 * Si queremos que conserve su tamaño original, tendremos que indicarlo manualmente.
 *
 * Un botón puede contener imagen y texto al mismo tiempo.
 * En el ejemplo, para que sólo tengan icono, el texto se ha eliminado.
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

void MainWindow::on_pushButton_1_clicked()
{
    //Extraemos la imagen del archivo de recursos
    QPixmap image = QPixmap(":/images/icons/Button-Log-Off-icon.png");
    //La asignamos al label
    ui->label->setPixmap(image);
}


void MainWindow::on_pushButton_2_clicked()
{
    QPixmap image = QPixmap(":/images/icons/Button-Turn-Off-icon.png");
    ui->label->setPixmap(image);
}

void MainWindow::on_pushButton_3_clicked()
{
    QPixmap image = QPixmap(":/images/icons/Button-Turn-On-icon.png");
    ui->label->setPixmap(image);
}
