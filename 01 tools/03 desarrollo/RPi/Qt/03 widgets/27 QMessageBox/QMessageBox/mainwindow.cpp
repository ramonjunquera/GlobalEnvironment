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

void MainWindow::on_action1_triggered()
{
    //Pulsado el botón 1
    //Ventama de mensaje del tipo about

    //Mostramos una ventana de Acerca de
    QMessageBox::about(this,"About","Se ha pulsado la opción 1");
}

void MainWindow::on_action2_triggered()
{
    //Pulsado el botón 2
    //Ventana de mensaje del tipo informativo

    //Mostramos una ventana informativa
    //QMessageBox::information(this,"Info","Se ha pulsado la opción 2");

}

void MainWindow::on_action3_triggered()
{
    //Pulsado el botón 3
    //Ventana de mensaje del tipo crítico

    //Mostramos una ventana crítica
    QMessageBox::critical(this,"Critical","Se ha pulsado la opción 3");
}

void MainWindow::on_action4_triggered()
{
    //Pulsado el botón 4
    //Ventana de mensaje del tipo alerta

    //Mostramos una ventana crítica
    QMessageBox::warning(this,"Warning","Se ha pulsado la opción 4");
}

void MainWindow::on_action5_triggered()
{
    //Pulsado el botón 5
    //Ventana de mensaje del tipo pregunta
    //Se gestiona la respuesta

    //Creamos variable para recoger el resultado de una ventana de
    //pregunta típica que tiene los botones Yes & No
    QMessageBox::StandardButton answer;
    //Mostramos una ventana de pregunta y recogemos su respuesta
    answer=QMessageBox::question(this,"Question","Se ha pulsado la opción 5");
    //Si se ha respondido que sí...lo indicamos con un mensaje informativo
    if(answer==QMessageBox::Yes) QMessageBox::information(this,"SI","Se ha respondido que sí");
    //En cualquier otro caso...también lo indicamos con un mensaje crítico
    else QMessageBox::critical(this,"NO","Se ha respondido No o se ha cancelado");
}

void MainWindow::on_action6_triggered()
{
    //Pulsado el botón 6
    //Ventana de mensaje del tipo pregunta con botones personalizados
    //Se gestiona la respuesta

    //Creamos variable para recoger el resultado de una ventana
    QMessageBox::StandardButton answer;
    //Mostramos una ventana de pregunta con botones personalizados:
    //Yes, No y Cancel.
    //Recogemos su respuesta
    answer=QMessageBox::question(this,"Question","Se ha pulsado la opción 6",QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
    //Si se ha respondido que sí...lo indicamos con un mensaje informativo
    if(answer==QMessageBox::Yes) QMessageBox::information(this,"SI","Se ha respondido que sí");
    //Si se ha respondido que no...lo indicamos con un mensaje informativo
    else if(answer==QMessageBox::No) QMessageBox::information(this,"NO","Se ha respondido que no");
    //Si se ha respondido Cancel o se ha cerrado la ventana...
    //...lo indicamos con un mensaje crítico
    else QMessageBox::critical(this,"CANCEL","Se ha cancelado");
}

void MainWindow::on_action7_triggered()
{
    //Pulsado el botón 7
    //Ventana de mensaje personalizada

    //Creamos el objeto
    QMessageBox msgBox;
    //Fijamos el texto del mensaje
    msgBox.setText("Se ha pulsado la opción 7");
    //Fijamos el título de la ventana
    msgBox.setWindowTitle("Info");
    //Añadimos los botones
    //Es importante tener en cuenta el orden en el que se añaden
    msgBox.addButton("Sí",QMessageBox::YesRole); //0
    msgBox.addButton("Nooo",QMessageBox::NoRole); //1
    msgBox.addButton("Cancelar",QMessageBox::RejectRole); //2
    //Definimos cuál es el botón que tiene el mismo significado
    //que cerrar la ventana
    msgBox.setEscapeButton(QMessageBox::Cancel);
    //Definimos el botón por defecto
    msgBox.setDefaultButton(QMessageBox::Yes);
    //Personalizamos el icono.
    //Si no lo hacemos, no aparecerá icono.
    //Cargamos una imagen del archivo de recursos y la guardamos
    //En un QPixmap
    QPixmap myIcon(":/icons/icons/7-24x24.png");
    //Asignamos el QPixmap a la ventana de mensaje como icono
    msgBox.setIconPixmap(myIcon);
    //Mostramos la ventana y recogemos su resultado
    int answer=msgBox.exec();
    //El resultado representa el número de botón
    //Los botones se numeran por el orden en el que se han añadido
    //En nuestro caso:
    // 0 = Yes
    // 1 = No
    // 2 = Cancel
    //Si se ha respondido que sí...lo indicamos con un mensaje informativo
    if(answer==0) QMessageBox::information(this,"SI","Se ha respondido que sí");
    //Si se ha respondido que no...lo indicamos con un mensaje informativo
    else if(answer==1) QMessageBox::information(this,"NO","Se ha respondido que no");
    //Si se ha respondido Cancel o se ha cerrado la ventana...
    //...lo indicamos con un mensaje crítico
    else QMessageBox::critical(this,"CANCEL","Se ha cancelado");
}
