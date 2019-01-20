#include "derivedclass.h"
#include <iostream> //Para cout

using namespace std; //Para cout

void DerivedClass::show()
{
    //Muestra desde dónde se está ejecutando
    cout << "Ejecutando desde la clase derivada" << endl;
}
