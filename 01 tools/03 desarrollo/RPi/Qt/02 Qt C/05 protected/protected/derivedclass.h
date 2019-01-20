#ifndef DERIVEDCLASS_H
#define DERIVEDCLASS_H

#include <baseclass.h>

class DerivedClass : public BaseClass
{
public:
    DerivedClass(); //Constructor
    int getProtected();
};

#endif // DERIVEDCLASS_H
