#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //Inicializamos el QPixmap creando uno nuevo con las mismas dimensiones que el QLabel
    myPixmap = new QPixmap(100,100);
    //Creamos un nuevo QPainter asociado al QPixmap
    myPainter = new QPainter(myPixmap);
    //Limpiamos la imágen
    clear();
    //Conectamos las signal de QLabelMouse con los slots
    connect(ui->label,SIGNAL(mousePressSignal(QPoint&,int&)),this,SLOT(mousePressSlot(QPoint&,int&)));
    connect(ui->label,SIGNAL(mouseMoveSignal(QPoint&)),this,SLOT(mouseMoveSlot(QPoint&)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::clear()
{
    //Borra la imágen

    //Borramos la imágen dibujando un rectángulo blanco que ocupe
    //toda el área
    myPainter->fillRect(0,0,99,99,QColor(255,255,255));
    //Asignamos el QPixmap al QLabel
    ui->label->setPixmap(*myPixmap);
}

void MainWindow::on_pushButton_clicked()
{
    //Se ha pulsado el botón clear

    //Simplemente limpia la imágen
    clear();
}

void MainWindow::mousePressSlot(QPoint &xy, int &button)
{
    //Se ha recibido una signal de que se ha pulsado el botón del mouse

    //Si el botón es uno de los que nos interesa...
    //Qt::LeftButton = 1
    //Qt::RightButton = 2
    //Qt::MidButton = 4
    if(button<3)
    {
        //Definimos el color
        //Negro (0,0,0) para el botón 1
        //Blanco (255,255,255) para el botón 2
        int c=(button-1)*255;
        //Lo asignamos al lápiz
        myPainter->setPen(QColor(c,c,c));
        //A partir de ahora lo que se dibuje se hará de este color
        //Llamamos al slot de puntero movido para que dibuja el
        //pixel actual
        mouseMoveSlot(xy);
    }
}

void MainWindow::mouseMoveSlot(QPoint &xy)
{
    //Se ha recibido una signal de que se ha movido el mouse

    //Dibujaremos un pixel en la posición del puntero del color
    //del lápiz
    myPainter->drawPoint(xy);
    //Asignamos l QPixmap al QLabelMouse
    ui->label->setPixmap(*myPixmap);
}
