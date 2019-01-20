#include <QCoreApplication>
#include <iostream> //Para cout

using namespace std; //Para cout

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    //Mostramos el texto en pantalla
    cout << "Hello world" << endl;

    return a.exec();
}
