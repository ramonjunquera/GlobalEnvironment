#include "rect2.h"

Rect2::Rect2(int width,int height)
{
    //Constructor

    //Guardamos los parámetros en las variables privadas
    _width=width;
    _height=height;
}

int Rect2::getArea()
{
    //Devuelve al área
    return _width*_height;
}
