#include "mainwindow.h"
#include <loginWindow.h>
#include <QApplication>

int main(int argc, char *argv[])
{
    //Es obligatorio crear un objeto de aplicación
    QApplication myApp(argc, argv);

    //Creamos la ventana de login
    LoginWindow *l=new LoginWindow;
    //La abrimos y anotamos el resultado
    bool credentialsOk=l->begin();
    //Ya no necesitamos el objeto de la ventana de login
    delete l;
    //Si no se han validado las credenciales...hemos terminado
    if(!credentialsOk) return 0;
    //Creamos la ventana principal
    MainWindow w;
    //La mostramos
    w.show();
    //Terminamos
    return myApp.exec();
}
