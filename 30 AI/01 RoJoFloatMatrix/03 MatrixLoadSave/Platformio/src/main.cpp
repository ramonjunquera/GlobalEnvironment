/*
  Autor: Ramón Junquera
  Fecha: 20200912
  Tema: AI
  Objetivo: Demo de librería RoJoFloatMatrix
  Descripción:
    Ejemplo de recuperación y guardado de matrices.
    La librería nos permita utilizar sistemas de archivo internos como
    SPIFFS para la famila ESP, o externos como SD para cualquiera.
    Pasos:
    - Creamos una matriz
    - La llenamos con valores
    - Mostramos la matriz
    - Intentamos recuperar los datos desde un archivo
    - Mostramos el estado de la carga
    - Mostramos la matriz
    - Guardamos la matriz en un archivo
    - Mostrmos el estado del guardado
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
  delay(1000);
  RoJoFloatMatrix A;
  //Asignamos valores iniciales a la matriz
  A.redim(3,2);
  A.m[0][0]=1;  A.m[0][1]=2;
  A.m[1][0]=-4; A.m[1][1]=3;
  A.m[2][0]=0;  A.m[2][1]=7;

  Serial.println();
  showMatrix(&A);
  Serial.println();

  switch(A.load("/myMatrix.flm")) {
    case 0:
      Serial.println("Load Ok");
      break;
    case 1:
      Serial.println("File not found");
      break;
    case 2:
      Serial.println("File corrupt");
      break;
    case 3:
      Serial.println("Out of memory");
      break;
  }

  Serial.println();
  showMatrix(&A);
  Serial.println();

  if(A.save("/myMatrix.flm")) Serial.println("Save Ok");
  else Serial.println("Save error");
}

void loop() {
  //Nada especial que hacer aquí
}