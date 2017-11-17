/*
 * Ejemplo de uso de imágen en objeto label y de librerías de recursos
 *
 * En QT no existe un objeto gráfico que muestre una imagen.
 * En estos casos se utiliza un label.
 *
 * El ejemplo muestra cómo cambiar la imagen/texto de un label desde programa.
 *
 * El objeto de imágen puede ser cargado directamente de un archivo contenido en
 * un lugar al que se tenga acceso.
 * El problema es que si el archivo se elimina, en tiempo de ejecución no se
 * podrá encontrar, ni cargar ni mostrar.
 *
 * Para evitar esto, se deben cargar las imágenes dentro del ejecutable.
 * Esto se hace con archivos de recursos.
 * Para crear uno nuevo en un proyecto, hacemos click con el botón derecho
 * sobre el nombre del proyecto, y seleccionamos
 *   Add New.../Qt/Qr Resource file/Choose...
 * Le damos un nombre al archivo.
 * Habitualmente se guarda en la misma carpeta que los fuentes.
 * Pulsamos el botón Next>
 * Confirmamos que se debe añadir al proyecto actual pulsando Finish.
 * Se creará una nueva carpeta en la estructura del proyecto llamada Resources
 * que contendrá el archivo de recursos (.qrc) con el nombre indicado.
 *
 * La ventana queda en disposición  de editar el archivo de recursos.
 * Siempre podemos volver a este punto haciendo click con el botón derecho sobre
 * el archivo de recursos y seleccionando "Open in Editor"
 *
 * Un archivo de recursos puede contener cualquier tipo de archivo.
 * Para facilitar la gestión se organiza internamente en carpetas a las que llama prefijos.
 *
 * Inicialmente el está vacío, por lo tanto sólo podemos crear un prefijo.
 * No es obligatorio utilizar los prefijos por defecto.
 * En este ejemplo el prefijo de las imágenes es /MyImages
 * Una vez creada la estructura, podemos incluir los archivos.
 * Realmente no incluye los archivos en el momento en el que se indica, sino que anota
 * la referencia del archivo para incluirlo en tiempo de compilación.
 * Por eso es importante no eliminar los archivos físicos. Sino dará error al compilar
 * porque no puede encontrarlos.
 *
 * Los archivos pueden a su vez estar en carpetas distintas a la que alberga el
 * archivo de recursos. En este caso, se incluiár el path relativo.
 * En el ejemplo las imágenes se encuentran en una carpeta llamada icons.
 * Por lo tanto, el path completo sería /MyImages/icons/FileName.ext
 *
 * Para distinguir una referencia externa de una interna (archivo de recursos) usaremos
 * el carácter : (dos puntos) al principio de todas las referencias internas.
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

void MainWindow::on_pushButton_Image1_clicked()
{
    //Pulsado botón para imagen 1

    //Creamos un objeto imagen
    QPixmap image(":/MyImages/icons/Button-Log-Off-icon.png");
    //Lo asignamos al label
    ui->label->setPixmap(image);
}

void MainWindow::on_pushButton_Image2_clicked()
{
    //Pulsado botón para imagen 2

    QPixmap image(":/MyImages/icons/Button-Turn-Off-icon.png");
    ui->label->setPixmap(image);
}

void MainWindow::on_pushButton_Image3_clicked()
{
    //Pulsado botón para imagen 3

    QPixmap image(":/MyImages/icons/Button-Turn-On-icon.png");
    ui->label->setPixmap(image);
}

void MainWindow::on_pushButton_Text_clicked()
{
    //Pulsado el botón Text

    //Asignamos un texto al label
    ui->label->setText("Hello");
}

void MainWindow::on_pushButton_empty_clicked()
{
    //Pulsado el boótn para vaciar el label

    //Si fuese de texto, Sería suficiente con asignar un QString vacío.
    //Como no sabemos qué muestra, le asignamos una imagen vacía.

    ui->label->setPixmap(QPixmap());
}
