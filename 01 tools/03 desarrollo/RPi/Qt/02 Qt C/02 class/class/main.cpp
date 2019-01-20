#include <QCoreApplication>
#include <iostream> //Para cout
#include <multi.h>

using namespace std; //Para cout

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    //Creamos objeto con instancia de clase
    Multi m(3);
    //Mostramos el resultado de la multiplicación
    cout << m.multiplica(4) << endl;

    return a.exec();
}
