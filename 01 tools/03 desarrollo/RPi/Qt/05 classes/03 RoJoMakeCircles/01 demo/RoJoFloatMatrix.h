/*
 * Autor: Ramón Junquera
 * Decha: 20200611
 * Tema: Gestión de matrices bidimensionales de tipo float
 */

#ifndef RoJoFloatMatrix_h
#define RoJoFloatMatrix_h

#include <stdint.h> //Para tipos standard
#include <cstdlib> //Para rand()
#include <time.h> //Para time()

class RoJoFloatMatrix {
  private:
    uint32_t _cols=0; //Número de columnas. Inicialmente ninguna
    uint32_t _rows=0; //Número de filas. Inicialmente ninguna
  public:
    float **m; //Puntero a array de arrays: matriz
    void end(); //Descarga memoria
    ~RoJoFloatMatrix(); //Destructor
    bool redim(uint32_t rows,uint32_t cols); //Redimensionar
    RoJoFloatMatrix(uint32_t rows=0,uint32_t cols=0); //Constructor con tamaño
    RoJoFloatMatrix(const RoJoFloatMatrix &A); //Constructor de asignación
    uint32_t cols(); //Número de columnas
    uint32_t rows(); //Número de filas
    RoJoFloatMatrix& T(); //Traspuesta
    bool maxmin(float *min,float *max); //Calcula máx y min
    bool fill(float minValue,float maxValue); //Llena con aleatorios
    RoJoFloatMatrix& operator + (RoJoFloatMatrix &A); //Suma matricial
    RoJoFloatMatrix& operator * (float v); //Producto escalar
    RoJoFloatMatrix& operator * (RoJoFloatMatrix &A); //Producto matricial
    RoJoFloatMatrix& f(float (*f)(float)); //Aplicación de fórmula
    RoJoFloatMatrix& operator = (RoJoFloatMatrix &A); //Asignación de valor
};

#endif
