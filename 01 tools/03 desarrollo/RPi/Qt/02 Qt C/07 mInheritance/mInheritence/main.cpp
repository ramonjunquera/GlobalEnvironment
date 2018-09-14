#include <QCoreApplication>
#include <iostream> //Para cout
#include <rect.h>

using namespace std;

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    //Creamos un objeto Rect
    Rect r(3,4);
    //Calculamos su área
    int area=r.area();
    //Calculamos su coste de pintar
    int cost=r.costArea(area);
    //Mostramos los resultados
    cout << "Área = " << area << endl;
    cout << "Coste = " << cost << endl;

    return a.exec();
}
