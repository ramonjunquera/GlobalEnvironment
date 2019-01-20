#include <QCoreApplication>
#include <iostream>
#include <rect1.h>
#include <square1.h>
#include <rect2.h>
#include <square2.h>
#include <rect3.h>
#include <square3.h>
#include <rect4.h>
#include <square4.h>

using namespace std;

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    Rect1 r1;
    r1.setMeasures(3,4);
    cout << "Area r1 = " << r1.getArea() << endl;

    Square1 s1;
    s1.setSide(5);
    cout << "Area s1 = " << s1.getArea() << endl;

    Rect2 r2(3,4);
    cout << "Area r2 = " << r2.getArea() << endl;

    Square2 s2(5);
    cout << "Area s2 = " << s2.getArea() << endl;

    Rect3 r3;
    r3.setMeasures(3,4);
    cout << "Area r3 = " << r3.getArea() << endl;

    Square3 s3;
    s3.setSide(5);
    cout << "Area s3 = " << s3.getArea() << endl;

    //Ejemplo para comprobar que funciona la llamada el destructor
    Square3 *sp;
    sp=new Square3();
    sp->setSide(5);
    cout << "Area sp = " << sp->getArea() << endl;
    delete sp;

    Rect4 r4(3,4);
    cout << "Area r4 = " << r4.getArea() << endl;

    Square4 s4(5);
    cout << "Area s4 = " << s4.getArea() << endl;

    return a.exec();
}
