#include "dialogwindow.h"
#include "ui_dialogwindow.h"

DialogWindow::DialogWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogWindow)
{
    ui->setupUi(this);
    _ok=false; //Inicialmente no se ha aceptado la ventana
}

DialogWindow::~DialogWindow()
{
    delete ui;
}

bool DialogWindow::open(QString *answer)
{
    //Se solicita abrir la ventana

    //Se abre la ventana en modal
    this->exec();
    //Hasta que la ventana no se cierra no se ejecuta esta línea
    //Si la ventana se ha aceptado...
    if(_ok) *answer=ui->lineEdit->text();
    //Devolvemos si la ventana ha sido aceptada
    return _ok;
}

void DialogWindow::on_pushButtonOk_clicked()
{
    //Se ha pulsado el botón Ok

    //La ventana se ha aceptado
    _ok=true;
    //Ocultamos la ventana para devolver el control
    this->hide();
}
