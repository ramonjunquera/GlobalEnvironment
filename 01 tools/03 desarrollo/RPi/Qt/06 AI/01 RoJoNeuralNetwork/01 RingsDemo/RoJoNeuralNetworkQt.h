/*
 * Autor: Ramón Junquera
 * Fecha: 20200911
 * Tema: Gestión de redes neuronales
 */

#ifndef RoJoNeuralNetworkQt_h
#define RoJoNeuralNetworkQt_h

#include <stdint.h> //Para tipos standard
#include <math.h> //Para exp() en funciones de activación
#include <RoJoFloatMatrixQt.h> //Gestión de matrices

//Definición de algunas funciones de activación
float sigmoidF(float x); //Sigmoidea
float sigmoidD(float x); //Derivada de sigmoidea
float reluF(float x); //ReLU
float reluD(float x); //Derivada de ReLU

//Definición de función de error cuadrático para función de coste
float cuadErrorF(float valueRight,float valueGot);
float cuadErrorD(float valueRight,float valueGot); //Derivada

//Gestión de capas de red neuronal
class RoJoNeuralLayerQt {
  public:
    RoJoFloatMatrixQt _W; //Coeficientes
    RoJoFloatMatrixQt _b; //BIAS (término independiente)
    uint16_t inNodes=0; //Número de nodos de entrada
    uint16_t outNodes=0; //Número de nodos de salida/internos
    bool begin(uint16_t inNodes,uint16_t outNodes); //Init
    void end();
    ~RoJoNeuralLayerQt(); //Destructor
    RoJoFloatMatrixQt *inValues; //Valores de entrada
    RoJoFloatMatrixQt outValues; //Valores de salida (aplicada función de activación)
    RoJoFloatMatrixQt sumW; //Resultados de suma ponderada (weighted sum)
    bool calc(float (*actF)(float)); //Calcula valores de salida
    RoJoFloatMatrixQt delta;
};

//Gestión de red neuronal
class RoJoNeuralNetworkQt {
  private:
    int16_t _layersCount=0; //Número de capas (incluida la de entrada)
    RoJoNeuralLayerQt* *_layers; //Puntero a array de punteros de objetos capa
    float _lr; //learning rate (ratio de aprendizaje)
    float (*_costF)(float,float); //Puntero de función de coste
    float (*_costFD)(float,float); //Puntero de derivada de función de coste
    float (*_actF)(float); //Puntero de función de activación
    float (*_actFD)(float); //Puntero de derivada de función de activación
  public:
    void end();
    ~RoJoNeuralNetworkQt(); //Destructor
    bool begin(RoJoFloatMatrixQt *topology,float (*actF)(float),float (*costF)(float,float)); //Init
    bool calc(RoJoFloatMatrixQt *inValues,RoJoFloatMatrixQt *outValues); //Calcula valores de salida
    bool beginTrain(float (*actFD)(float),float (*costFD)(float,float),float lr=0.05); //Init train
    bool train(RoJoFloatMatrixQt *inValues,RoJoFloatMatrixQt *rightValues,RoJoFloatMatrixQt *outValues); //Entrenamiento
    bool cost(RoJoFloatMatrixQt *outValues,RoJoFloatMatrixQt *rightValues,RoJoFloatMatrixQt *meanCost); //Calcula error
    bool mesh(RoJoFloatMatrixQt *cXb,RoJoFloatMatrixQt *cYb,RoJoFloatMatrixQt *Vb); //Calculamos los valores de mesh
};

#endif
