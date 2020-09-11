/*
 * Autor: Ramón Junquera
 * Fecha: 20200911
 * Tema: Gestión de matrices bidimensionales de tipo float
 */

#ifndef RoJoFloatMatrixQt_h
#define RoJoFloatMatrixQt_h

#include <stdint.h> //Para tipos standard
#include <cstdlib> //Para rand()
#include <time.h> //Para time() inicializando semilla de números aleatorios

class RoJoFloatMatrixQt {
  protected:
    uint16_t _rows=0; //Número de filas. Inicialmente ninguna
    uint16_t _cols=0; //Número de columnas. Inicialmente ninguna
    void _mCopy(RoJoFloatMatrixQt *A,uint16_t deltaRows,uint16_t deltaCols); //Copia con delta
  public:
    float **m; //Puntero a array de arrays: matriz
    void end(); //Descarga memoria
    ~RoJoFloatMatrixQt(); //Destructor
    bool redim(uint16_t rows,uint16_t cols); //Redimensionar
    RoJoFloatMatrixQt(uint16_t rows=0,uint16_t cols=0); //Constructor con tamaño
    RoJoFloatMatrixQt(RoJoFloatMatrixQt &A); //Constructor de asignación
    uint16_t cols(); //Número de columnas
    uint16_t rows(); //Número de filas
    bool T(RoJoFloatMatrixQt *A); //Traspuesta
    bool glueRows(RoJoFloatMatrixQt *A,RoJoFloatMatrixQt *B); //Pega filas de A y B
    bool glueCols(RoJoFloatMatrixQt *A,RoJoFloatMatrixQt *B); //Pega columnas de A y B
    bool maxminRangeCol(float *min,float *max,uint16_t firstCol,uint16_t lastCol); //Calcula máx y min de un rango de columnas
    bool maxmin(float *min,float *max); //Calcula máx y min
    bool maxminCol(float *min,float *max,uint16_t col); //Calcula máx y min de una columna
    bool fillRand(float minValue=-1,float maxValue=1); //Llena con aleatorios
    bool fill(float value); //Llena con una valor
    bool linspace(float minValue,float maxValue,uint16_t samples); //Rango
    bool f1(float (*f)(float),RoJoFloatMatrixQt *p); //Aplicación de fórmula de 1 parámetro
    bool f2(float (*f)(float,float),RoJoFloatMatrixQt *p0,RoJoFloatMatrixQt *p1); //Aplicación de fórmula de 2 parámetros
    bool eProd(RoJoFloatMatrixQt *A,RoJoFloatMatrixQt *B); //Producto escalar
    bool eProd(float v); //Producto escalar
    bool mProd(RoJoFloatMatrixQt *A,RoJoFloatMatrixQt *B); //Producto matricial
    bool mSum(RoJoFloatMatrixQt *A); //Suma matricial
    bool mCopy(RoJoFloatMatrixQt *A); //Copia matricial
    bool sumCol(RoJoFloatMatrixQt *A); //Suma la matriz columna a todas las columnas
    bool sumRow(RoJoFloatMatrixQt *A); //Suma la matriz fila a todas las filas
    uint16_t* scrambleList(); //Crea lista desordenada
    bool scramble(uint16_t *sList); //Desordena la matriz en base a la lista
    bool meanCols(RoJoFloatMatrixQt *A); //Valor promedio de las columnas
};

#endif
