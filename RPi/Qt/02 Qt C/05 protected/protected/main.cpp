#include <QCoreApplication>
#include <iostream> //Para cout
#include <derivedclass.h>

using namespace std; //Para cout

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    DerivedClass d;
    cout << "Ejemplo de public " << d.publicVar << endl;
    cout << "Ejemplo de protected " << d.getProtected() << endl;

    return a.exec();
}

