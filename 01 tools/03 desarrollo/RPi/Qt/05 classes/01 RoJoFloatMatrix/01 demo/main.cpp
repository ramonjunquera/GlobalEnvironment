/*
 * Autor: Ramón Junquera
 * Decha: 20200609
 * Tema: Demo de clase de gestión de matrices bidimensionales de tipo float
 */

#include <iostream>
#include "RoJoFloatMatrix.h"

using namespace std;

void printMatrix(RoJoFloatMatrix *A) {
  for(uint32_t row=0;row<A->rows();row++) {
    for(uint32_t col=0;col<A->cols();col++) {
      cout << A->m[row][col] << "\t";
    }
    cout << endl;
  }
}

float doble(float v) {
  return 2*v;
}

int main() {
  RoJoFloatMatrix A(3,2); //Creamos una matriz
  cout << "(rows,cols)=(" << A.rows() << "," << A.cols() << ")" << endl;
  //Llenamos sus valores
  for(uint32_t row=0;row<A.rows();row++) {
    for(uint32_t col=0;col<A.cols();col++) {
      A.m[row][col]=10*row+col;
    }
  }
  cout << "A" << endl;
  printMatrix(&A); //Mostramos su contenido

  RoJoFloatMatrix B=A; //Creamos la matrix B como copia de A
  cout << "B=A" << endl;
  printMatrix(&B); //Mostramos su contenido

  RoJoFloatMatrix C=A+B; //Creamos la matrix C como suma de A y B
  cout << "C=A+B" << endl;
  printMatrix(&C); //Mostramos su contenido

  B=A.T(); //Trasponemos A y la guardamos en B
  cout << "B=A.T" << endl;
  printMatrix(&B); //La mostramos

  A.fill(-10,10); //Rellenamos la matriz A con valores aleatorios
  cout << "A=random" << endl;
  printMatrix(&A); //Mostramos su contenido

  B=A*(-1);
  cout << "B=A*(-1)" << endl;
  printMatrix(&B); //Mostramos su contenido

  A.redim(2,4);
  A.m[0][0]=5;
  A.m[0][1]=3;
  A.m[0][2]=-4;
  A.m[0][3]=-2;
  A.m[1][0]=8;
  A.m[1][1]=-1;
  A.m[1][2]=0;
  A.m[1][3]=-3;
  cout << "A" << endl;
  printMatrix(&A); //Mostramos su contenido
  B.redim(4,3);
  B.m[0][0]=1;
  B.m[0][1]=4;
  B.m[0][2]=0;
  B.m[1][0]=-5;
  B.m[1][1]=3;
  B.m[1][2]=7;
  B.m[2][0]=0;
  B.m[2][1]=-9;
  B.m[2][2]=5;
  B.m[3][0]=5;
  B.m[3][1]=1;
  B.m[3][2]=4;
  cout << "B" << endl;
  printMatrix(&B); //Mostramos su contenido
  C=A*B;
  cout << "C=A*B" << endl;
  printMatrix(&C); //Mostramos su contenido

  A=C.f(doble);
  cout << "A=C.f(doble)" << endl;
  printMatrix(&A); //Mostramos su contenido

  //Liberamos las variables de matrices
  A.end();
  B.end();
  C.end();

  //Todo ok
  return 0;
}
