#include "multi.h"

Multi::Multi(int coef)
{
    //Constructor

    //Guarda el coeficiente indicado
    _coef=coef;
}

int Multi::multiplica(int valor)
{
    //Devuelve el resultado de multiplicar el valor dado por el coeficiente
    return _coef*valor;
}
