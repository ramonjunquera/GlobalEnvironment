#include <QCoreApplication>
#include <point.h>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    //Creamos una instancia de la clase
    Point p(2,3);
    //Mostramos las variables privadas
    p.show();

    return a.exec();
}
