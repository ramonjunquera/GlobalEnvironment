/*
  Autor: Ramón Junquera
  Fecha: 20221201
  Tema: Configuración de proyecto c++ en Visual Code
  Descripción:
  Se intenta crear un proyecto simple en c++ que utilice librerías y
  acceso a datos que tenga un aspecto similar a un proyecto de Arduino.

  Notas:
  - El programa principal se ubica en la carpeta raíz del proyecto.
    En el ejemplo main.cpp
  - El ejecutable se creará en la carpeta raíz del proyecto.
    Tendrá el mismo nombre, pero sin extensión.
  - El tomará la carpeta data como raíz del sistema de archivos.
  - La carpeta lib contendrá las distintas librerías utilizadas, cada una
    de ellas en su propia carpeta. En el ejemplo MyClass & MyClass1.
  - La carpeta .vscode es obligatoria. Contiene los archivos de configuración
    que definen las carpetas.
  - Cuando se cambian los includes, se debe modificar el archivo tasks.json
*/

#include <iostream> //Para mensajes cout
#include <MyClass.h>
#include <MyClass1.h>

using namespace std;

int main(int argc, char **argv) {
  cout << "hola" << endl;
}
