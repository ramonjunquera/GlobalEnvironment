#include "square1.h"

void Square1::setSide(int side)
{
    //Asigna valor del lado

    //Utilizamos el método se asignar dimensiones de la clase base
    Rect1::setMeasures(side,side);
}
