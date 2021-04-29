#include "square3.h"

Square3::Square3():Rect3()
{
    //Nada especial que hacer aquí
    //En la declaración ya se está llamando al constructor del la clase base
    //para que reserve la memoria
}

void Square3::setSide(int side)
{
    //Asigna las dimensiones

    //Lo llamamos haciendo referencia a la clase base
    Rect3::setMeasures(side,side);
}

Square3::~Square3()
{
    //Destructor

    //Nada especial que hacer aquí
    //Ya se llama automáticamente al destructor de la clase base
    //Pero es necesario crearlo
}

