//Pendiente
//La carpeta inicial debe ser la de ejecución de la aplicación

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>

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

void MainWindow::on_pushButtonFile_clicked()
{
    //Se ha pulsado el botón de selección de un archivo

    //Calculamos el path actual del ejecutable
    QDir dir;
    QString currentPath=dir.absolutePath();
    //Pedimos que se seleccione un único archivo
    QString fileName = QFileDialog::getOpenFileName(this,tr("Selecciona archivo"),currentPath,tr("Todos los archivos (*.*)"));
    //Si no se ha cancelado...anotamos el archivo seleccionado en el cuadro de texto
    if(!fileName.isNull()) ui->plainTextEditFile->document()->setPlainText(fileName);
}

void MainWindow::on_pushButtonFiles_clicked()
{
    //Se ha pulsado el botón de selección de varios archivos

    //Definimos la variable que contendrá todos los archivos seleccionados separados por \n
    QString filesConcat="";
    //Calculamos el path actual del ejecutable
    QDir dir;
    QString currentPath=dir.absolutePath();
    //Pedimos que se seleccionen varios archivos
    QStringList files=QFileDialog::getOpenFileNames(this,tr("Selecciona varios archivos"),currentPath,tr("Imágenes (*.png *.jpg *.bmp);;Todos los archivos (*.*)"));
    //Si se ha seleccionado algo...
    if(!files.isEmpty())
    {
        //...recorremos todos los items con un iterator
        for(auto itr=files.begin();itr!=files.end();itr++)
            //Añadimos a la cadena final el path del archivo procesado
            filesConcat+=*itr+"\n";
        //Asignamos la cadena al cuador de texto
        ui->plainTextEditFiles->document()->setPlainText(filesConcat);
    }
}

void MainWindow::on_pushButtonFolder_clicked()
{
    //Se ha pulsado el botón de selección de una carpeta

    //Calculamos el path actual del ejecutable
    QDir dir;
    QString currentPath=dir.absolutePath();
    //Pedimos que se seleccione una carpeta
    QString folderName=QFileDialog::getExistingDirectory(this,tr("Selecciona carpeta"),currentPath,QFileDialog::ShowDirsOnly);
    //Si se ha seleccionado algo...anotamos el path en el cuadro de texto
    if(!folderName.isNull()) ui->plainTextEditFolder->document()->setPlainText(folderName);
}

void MainWindow::on_pushButtonFileSave_clicked()
{
    //Se ha pulsado el botón de selección de un archivo para guardar

    //Calculamos el path actual del ejecutable
    QDir dir;
    QString currentPath=dir.absolutePath();
    //Pedimos que se seleccione el archivo para guardar
    QString fileName = QFileDialog::getSaveFileName(this,tr("Selecciona archivo para guardar"),currentPath+"/prueba.txt",tr("Archivos de texto (*.txt);;Todos los archivos (*.*)"));
    //Si no se ha cancelado...anotamos el archivo seleccionado en el cuadro de texto
    if(!fileName.isNull()) ui->plainTextEditFileSave->document()->setPlainText(fileName);
}
