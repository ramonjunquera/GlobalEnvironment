#include "square4.h"

Square4::Square4(int side):Rect4(side,side)
{
    //Nada especial que hacer aquí
    //En la declaración ya se está llamando al constructor del la clase base
    //para que reserve la memoria y asigne los parámetros
}

Square4::~Square4()
{
    //Destructor

    //Nada especial que hacer aquí
    //Ya se llama automáticamente al destructor de la clase base
    //Pero es necesario crearlo
}
