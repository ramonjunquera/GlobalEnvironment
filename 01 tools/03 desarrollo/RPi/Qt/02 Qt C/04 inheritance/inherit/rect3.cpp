#include "rect3.h"
#include <QDebug>

Rect3::Rect3()
{
    //Constructor

    //Reservamos memoria para los punteros privados
    _width = new int;
    _height = new int;
    qDebug() << "constructor Rect3";
}

void Rect3::setMeasures(int width, int height)
{
    //Asignación de dimensiones
    *_width=width;
    *_height=height;
}

Rect3::~Rect3()
{
    //Destructor

    //Liberamos la memoria de los punteros reservada en el constructor
    delete _width;
    delete _height;
    qDebug() << "destructor Rect3";
}

int Rect3::getArea()
{
    //Calcula el área
    return (*_width)*(*_height);
}
