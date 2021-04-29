#include "rect4.h"

Rect4::Rect4(int width,int height)
{
    //Constructor

    //Reservamos memoria para los punteros privados
    _width = new int;
    _height = new int;
    //Guardamos los parámetros en las variables privadas
    *_width=width;
    *_height=height;
}

Rect4::~Rect4()
{
    //Destructor

    //Liberamos la memoria de los punteros reservada en el constructor
    delete _width;
    delete _height;
}

int Rect4::getArea()
{
    //Calcula el área
    return (*_width)*(*_height);
}
