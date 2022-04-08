#include <QCoreApplication>
#include <QDebug>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    int x;
    qDebug() << "Creada variable x";
    x=5;
    qDebug() << "Asignado el valor" << x;

    return a.exec();
}
