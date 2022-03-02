#include "loginwindow.h"
#include "ui_loginwindow.h"

LoginWindow::LoginWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoginWindow)
{
    ui->setupUi(this);
}

LoginWindow::~LoginWindow()
{
    delete ui;
}

bool LoginWindow::begin()
{
    //Método para abrir la ventana
    //Devuelve si se ha conseguido hacer el login correcto.

    //Inicialmente las credenciales no se han validado
    _ok=false;
    //Abrimos la ventana como modal
    this->exec();
    //La ventana se ha cerrado
    //Devolvemos si las credenciales son correctas
    return _ok;
}

void LoginWindow::on_pushButtonOk_clicked()
{
    //Se ha pulsado el botón Ok

    //Si las credenciales coinciden...activamos el flag de credenciales correctas
    if(ui->lineEditUser->text()=="admin" && ui->lineEditPassword->text()=="admin") _ok=true;
    //Ocultamos la ventana y devolvemos el control
    hide();
}
