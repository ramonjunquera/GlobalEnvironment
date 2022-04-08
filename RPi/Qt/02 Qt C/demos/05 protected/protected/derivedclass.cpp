#include "derivedclass.h"

DerivedClass::DerivedClass():BaseClass()
{
    //Nada especial que hacer aquí
}

int DerivedClass::getProtected()
{
    return protectedVar;
}
