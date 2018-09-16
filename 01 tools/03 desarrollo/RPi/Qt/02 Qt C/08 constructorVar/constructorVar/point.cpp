#include "point.h"

void Point::show()
{
    //Muestra las variables privadas
    cout << "x=" << _x << ",y=" << _y << endl;
}

Point::Point(int x,int y):_x(x),_y(y)
{
    //Nada especial que hacer aquí porque los parámetros se han
    //asignado a las variables privadas en las definición del
    //constructor
}
