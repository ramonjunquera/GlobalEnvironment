#ifndef DERIVEDCLASS_H
#define DERIVEDCLASS_H

#include <baseclass.h>

class DerivedClass : public BaseClass
{
public:
    void show(); //Muestra desde dónde se está ejecutando el método
};

#endif // DERIVEDCLASS_H
