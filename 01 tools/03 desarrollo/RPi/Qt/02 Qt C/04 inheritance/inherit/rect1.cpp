#include "rect1.h"

void Rect1::setMeasures(int width,int height)
{
    //Asignar dimensiones

    //Guardamos los parámetros en las variables privadas
    _width=width;
    _height=height;
}

int Rect1::getArea()
{
    //Devuelve al área
    return _width*_height;
}
