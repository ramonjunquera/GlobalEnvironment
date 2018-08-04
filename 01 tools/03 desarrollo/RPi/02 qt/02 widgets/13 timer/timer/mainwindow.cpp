#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    //Constructor de la clase del formulario

    //Creamos una nueva instancia del timer
    myTimer = new QTimer(this);
    //Le asignamos la función a la que llamaár
    connect(myTimer,SIGNAL(timeout()),this,SLOT(TimerLauncher()));
    //Arrancamos el timer para que se ejecute cada segundo
    myTimer->start(1000);
    //Ahora ya podemos inicializar los objetos gráficos
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::TimerLauncher()
{
    //Función a la que llama el timer

    //Variable que guarda el estado de la bombilla
    static bool bulbStatus=false;
    //Cambiamos el estado de la bombilla
    bulbStatus=!bulbStatus;
    //Cargamos la imagen teniendo el cuenta el estado
    QPixmap image(bulbStatus?":/icons/icons/bulb-on-128.png":":/icons/icons/bulb-off-128.png");
    //Asignamos la imagen al label
    ui->label_bulb->setPixmap(image);
}
