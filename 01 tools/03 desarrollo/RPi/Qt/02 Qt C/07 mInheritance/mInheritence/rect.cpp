#include "rect.h"

Rect::Rect(int width,int height):Shape(width,height)
{
    //Constructor
    //Nada especial que hacer aquí
}

int Rect::area()
{
    //Calcula el área del rectángulo
    return _width*_height;
}
