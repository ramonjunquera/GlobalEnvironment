#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    modified=false; //Inicialmente no se ha modificado
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionSave_triggered()
{
    //Se ha seleccionado la opción de guardar

    //Simplemente borramos el flag de modificado
    modified=false;
}

void MainWindow::on_plainTextEdit_textChanged()
{
    //Ha cambiado el texto

    //Simplemente activamos el flag de modificado
    modified=true;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    //Se ha lanzado el evento de cerrado de ventana


    //Si hay modificaciones...
    if(modified)
    {
        //Creamos variable para recoger el resultado de una ventana
        QMessageBox::StandardButton answer;
        //Mostramos una ventana de pregunta con botones estándar: Yes & No
        //Recogemos su respuesta
        answer=QMessageBox::question(this,"Texto modificado","Hay cambios no guardados que se perderán.\nDesea continuar?");
        //Si no se acepta...ignoramos el evento
        if(answer!=QMessageBox::Yes) event->ignore();
    }
}
