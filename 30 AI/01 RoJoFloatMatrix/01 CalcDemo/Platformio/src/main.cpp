/*
  Autor: Ramón Junquera
  Fecha: 20210417
  Tema: Multitasking / FreeRTOS
  Objetivo: Demo de librería RoJoFloatMatrix
  Descripción:
    Demostración de uso de librería RoJoFloatMatrix.
    Se realizan varios cálculos matriciales.

    | 1  2  3|   | -4  3  1|   |19  7   16|
    |-4  3  5| * |  7 -1  0| = |52 -5   21|
    | 0  7 -3|   |  3  2  5|   |40 -13 -15|

    | 1  2  3|   | -4  3  1|   |-3  5  4|
    |-4  3  5| + |  7 -1  0| = | 3  2  5|
    | 0  7 -3|   |  3  2  5|   | 3  9  2|
  
Nota:
  Este ejemplo puede funcionar tanto con RoJoFloatMatrix
  como con RoJoFloatMatrixESP32
*/

#include <Arduino.h>
#include <RoJoFloatMatrix.h> //Gestión de matrices

void showMatrix(RoJoFloatMatrix *M) {
  for(uint16_t row=0;row<M->rows();row++) {
    for(uint16_t col=0;col<M->cols();col++) {
      Serial.printf("%f\t",M->m[row][col]);
    }
    Serial.println();
  }
}

void setup() {
  Serial.begin(115200); //Inicializamos el puerto serie
  RoJoFloatMatrix A,B,C; //Creamos matrices
  //Asignamos valores a las matrices
  A.redim(3,3);
  A.m[0][0]=1;  A.m[0][1]=2;  A.m[0][2]=3;
  A.m[1][0]=-4; A.m[1][1]=3;  A.m[1][2]=5;
  A.m[2][0]=0;  A.m[2][1]=7;  A.m[2][2]=-3;
  B.redim(3,3);
  B.m[0][0]=-4; B.m[0][1]=3;  B.m[0][2]=1;
  B.m[1][0]=7;  B.m[1][1]=-1; B.m[1][2]=0;
  B.m[2][0]=3;  B.m[2][1]=2;  B.m[2][2]=5;

  Serial.println("\nmProd");;
  C.mProd(&A,&B);
  showMatrix(&C);

  Serial.println("\nmSum");;
  A.mSum(&B);
  showMatrix(&A);

  Serial.println("\ncopia a C");
  C.mCopy(&A);
  showMatrix(&C);

  Serial.println("\nMatriz columna");;
  B.redim(3,1);
  B.m[0][0]=1;
  B.m[1][0]=2;
  B.m[2][0]=3;
  showMatrix(&B);

  Serial.println("\nResultado de sumar matriz columna");
  A.sumCol(&B);
  showMatrix(&A);

  Serial.println("\nMatriz fila");
  B.redim(1,3);
  B.m[0][0]=1; B.m[0][1]=2; B.m[0][2]=3;
  showMatrix(&B);

  Serial.println("\nResultado de sumar matriz fila");
  A.sumRow(&B);
  showMatrix(&A);

  Serial.println("\nScramble List");
  uint16_t *sList=A.scrambleList();
  for(uint16_t row=0;row<3;row++) Serial.println(sList[row]);

  Serial.println("\nMatriz reordenada");
  A.scramble(sList);
  delete[] sList;
  showMatrix(&A);

  Serial.println("\nMatriz traspuesta");
  C.T(&A);
  showMatrix(&C);
  
  float d=0;
  C.det(&d);
  Serial.printf("\nDeterminante=%f\n",d);

  Serial.println("\nCopiamos última columna de matriz reordenada a la primera de la traspuesta");
  C.copyCols(&A,2,0,1);
  showMatrix(&C);

  Serial.println("\nResolución de sistemas de ecuaciones lineales");
  Serial.println("x-y=2");
  Serial.println("2x+y=19");
  A.redim(2,3); //Redimensionamos matriz extendida
  A.m[0][0]=1;
  A.m[0][1]=-1;
  A.m[0][2]=2;
  A.m[1][0]=2;
  A.m[1][1]=1;
  A.m[1][2]=19;
  Serial.println("Matriz extendida:");
  showMatrix(&A);
  B.linearEq(&A);
  Serial.println("Soluciones:");
  showMatrix(&B);

  Serial.println("\nResolución de sistemas de ecuaciones lineales");
  Serial.println("x+y-z=-1");
  Serial.println("x+2y+2z=0");
  Serial.println("2x+y-z=1");
  A.redim(3,4); //Redimensionamos matriz extendida
  A.m[0][0]=1;
  A.m[0][1]=1;
  A.m[0][2]=-1;
  A.m[0][3]=-1;
  A.m[1][0]=1;
  A.m[1][1]=2;
  A.m[1][2]=2;
  A.m[1][3]=0;
  A.m[2][0]=2;
  A.m[2][1]=1;
  A.m[2][2]=-1;
  A.m[2][3]=1;
  Serial.println("Matriz extendida:");
  showMatrix(&A);
  B.linearEq(&A);
  Serial.println("Soluciones:");
  showMatrix(&B);

  Serial.println("\nSuma de columnas:");
  B.sumCols(&A);
  showMatrix(&B);

  Serial.println("\nPromedio de columnas:");
  B.meanCols(&A);
  showMatrix(&B);
}

void loop() {
  //Nada especial que hacer aquí
}