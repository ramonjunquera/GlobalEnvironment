/*
 * Autor: Ramón Junquera
 * Decha: 20200607
 * Tema: Sobrecarga de operadores
 * Objetivo: clase con operadores
 * Descripción:
 * Se crea una clase llamada Pareja con dos variables privadas.
 * En la misma definición de clase se sobrecarga el operador suma.
 */

#include <iostream>

using namespace std;

class Pareja {
  private:
    int _a,_b;
  public:
    //Asignación de valores a variables privadas
    void set(int a,int b) {
      _a=a;
      _b=b;
    }
    
    //Definición de operador +   
    Pareja operator + (const Pareja &p) {
      //Al objeto actual (Pareja) se le quiere sumar otro objeto Pareja
      //Añadimos los valores de las variables internas del parámetro
      //Devolvemos el objeto actual
      Pareja res; //Creamos un objeto de respuesta
      //Calculamos valores y los asignamos a variables privadas
      //Podríamos haber utilizado el método set
      //Lo hacemos así para comprobar que al ser la misma clase, tenemos
      //acceso directo a las variables privadas
      res._a=_a+p._a; 
      res._b=_b+p._b;
      return res; //Devolvemos el objeto creado como respuesta
    }

    //Muestra el contenido de las variables privadas
    void print() {
      cout << "a=" << _a << ", b=" << _b << endl;
    }
};

int main(int argc, char **argv) {
  Pareja A,B; //Creamos dos objetos Pareja
  A.set(1,2); //Asignamos valores a objeto A
  cout << "A: ";
  A.print(); //Los mostramos
  B.set(4,7); //Asignamos valores a objeto B
  cout << "B: ";
  B.print(); //Los mostramos
  Pareja C=A+B; //Creamos un nuevo objeto Pareja como suma de otros dos
  cout << "C: ";
  C.print(); //Mostramos su contenido
	
	//Todo ok
	return 0;
}

