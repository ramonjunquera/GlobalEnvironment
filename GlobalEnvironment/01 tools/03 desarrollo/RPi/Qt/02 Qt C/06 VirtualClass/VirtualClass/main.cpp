#include <QCoreApplication>
#include <baseclass.h>
#include <derivedclass.h>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    //Creamos una clase derivada
    DerivedClass d;
    //Mostramos el texto de la clase derivada
    d.show();
    //Creamos un puntero de una clase base
    BaseClass *b;
    //Asignamos el puntero de la clase derivada a la clase base
    //A partir de ahora se comportará como una clase base
    b=&d;
    //Mostramos el texto de la clase base
    //Dependerá de si el método show de la clase base es virtual o no
    b->show();

    return a.exec();
}
