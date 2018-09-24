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

void MainWindow::on_pushButtonText_clicked()
{
    //Se ha pulsado el botón de texto

    //Definimos la variable que nos indicará si el valor obtenido el válido
    bool ok=false;
    //Llamamos a una ventana de QInputDialog que obtiene un texto
    //Lo recogemos en una variable
    //Le asignamos título de ventana, texto de pregunta
    //El texto se mostrarán en pantalla mientras se escribe (no es una contraseña)
    //Asignamos el texto de respuesta por defecto
    //Indicamos puntero de variable con flag para saber si se ha aceptado la respuesta
    QString text=QInputDialog::getText(this,"Prueba input","Escribe un texto",QLineEdit::Normal,"texto por defecto",&ok);
    //Si la respuesa se ha aceptado...asignamos el texto al label
    if(ok) ui->labelText->setText(text);
}

void MainWindow::on_pushButtonInt_clicked()
{
    //Se ha pulsado el botón de entero

    //Llamamos a una ventana de QInputDialog que obtiene un texto
    //Lo recogemos en una variable
    //Asumimos el resto de parámetros:
    // - La respuesta por defecto es 0
    // - Los límites son los del tipo de variable int
    // - No controlamos si la centana se ha validado
    int i=QInputDialog::getInt(this,"Prueba input","Escribe un entero");
    //Convertimos el número en QString y la asignamos al label
    ui->labelInt->setText(QString::number(i));
}


void MainWindow::on_pushButtonDouble_clicked()
{
    //Se ha pulsado el botón double

    //Definimos la variable que nos indicará si el valor obtenido el válido
    bool ok=false;
    //Llamamos a una ventana de QInputDialog que obtiene un número double
    //Lo recogemos en una variable
    //Le asignamos título de ventana, texto de pregunta, valor por defecto
    //límite inferior, límite superior, número de decimales
    //Indicamos puntero de variable con flag para saber si se ha aceptado la respuesta
    double d=QInputDialog::getDouble(this,"Prueba input","Escribe un número decimal",33.456,-1000,1000,3,&ok);
    //Si la respuesa se ha aceptado...asignamos el número al label
    if(ok) ui->labelDouble->setText(QString::number(d));
}

void MainWindow::on_pushButtonMulti_clicked()
{
    //Se ha pulsado el botón de texto multi line

    //Definimos la variable que nos indicará si el valor obtenido el válido
    bool ok=false;
    //Llamamos a una ventana de QInputDialog que obtiene un texto multi línea
    //Lo recogemos en una variable
    //Le asignamos título de ventana, texto de pregunta, valor por defecto
    //Indicamos puntero de variable con flag para saber si se ha aceptado la respuesta
    QString m=QInputDialog::getMultiLineText(this,"Prueba input","Escribe texto en varias líneas","Texto por\ndefecto",&ok);
    //Si la respuesa se ha aceptado...asignamos el texto al label
    if(ok) ui->labelMulti->setText(m);
}

void MainWindow::on_pushButtonItem_clicked()
{
    //Se ha pulsado el botón de item

    //Creamos una lista de QString y la llenamos
    QStringList colores;
    colores << "rojo";
    colores << "verde";
    colores << "azul";
    colores << "blanco";
    colores << "negro";
    //Nota. colores << "rojo"; es lo mismo que colores.append("rojo");

    //Definimos la variable que nos indicará si el valor obtenido el válido
    bool ok=false;
    //Llamamos a una ventana de QInputDialog que obtiene un texto de una lista
    //Lo recogemos en una variable
    //Le asignamos título de ventana, texto de pregunta
    //La lista de opciones a seleccionar
    //El índice de la opción predeterminada (0=el primero)
    //Indicamos que la selección es editable. La lista de opciones sólo es
    //orientativa. Una vez seleccionado un valor, puede ser editado por el usuario
    //y se devolverá el valor editado. Si lo ponemos en false, la lista de
    //valores es fija y obligatoria.
    //Indicamos puntero de variable con flag para saber si se ha aceptado la respuesta
    QString c=QInputDialog::getItem(this,"Prueba input","Elige un color",colores,0,true,&ok);
    //Si la respuesa se ha aceptado...asignamos la opción seleccionada al label
    if(ok) ui->labelItem->setText(c);
}
