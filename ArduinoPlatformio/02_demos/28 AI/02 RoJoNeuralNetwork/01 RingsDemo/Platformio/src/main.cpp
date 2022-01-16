/*
  Autor: Ramón Junquera
  Fecha: 20201110
  Tema: Redes Neuronales
  Objetivo: Demo
  Material: M5Stack Fire

  Descripción:
    Haciendo uso de las distintas librerías, generaremos un par
    de anillos concéntricos y entrenaremos una red neuronal para
    que sea capaz de distinguir uno de otro.
    Se representa gráficamente el resultado.
    Mientras calcula los led periféricos están en rojo.
    Al terminar se vuelven verdes y esperan la pulsación del primer
    botón para generar un nuevo juego de datos y comenzar el cálculo
    de nuevo.
  Nota:
    Este desarrollo puede funcionar tanto con la librería RoJoFloatMatrix
    como con RoJoFloatMatrixESP32.
    La ventaja de la última es que es multitasking y aprovecha las dos
    CPUs del dispositivo, aumentando su velocidad de cálculo en un 40%.
*/

#include <Arduino.h>
#include <RoJoNeuralNetwork.h> //Redes neuronales
#include <RoJoSprPlt.h> //Representación gráfica de funciones
#include <RoJoILI9341.h> //Librería de gestión del display ILI9341
#include <RoJoFloatMatrix.h> //Gestión de matrices
#include <RoJoMakeCircles.h> //Creación de anillos
#include <RoJoNeoPixel.h> //Gestión de leds NeoPixel
#include <RoJoSwitch.h> //Librería de gestión de pulsadores

RoJoNeoPixel leds; //Objeto de gestión de leds NeoPixel
RoJoSwitch button1(39); //Primer botón
RoJoSprPlt plt; //Objeto de representación de gráfica de funciones
RoJoILI9341 display; //Objeto de gestión de display
RoJoFloatMatrix R0,R1; //Matrices de coordenadas de cada anillo
uint16_t samples=100; //Número de puntos de cada anillo
RoJoNeuralNetwork nn; //NeuralNetwork Object
RoJoFloatMatrix X; //Matriz de coordenadas de todos los puntos, para entrenamiento
RoJoFloatMatrix Z; //Matriz que guardará los valores de salida
RoJoFloatMatrix Y; //Matriz de valores correctos para el entrenamiento de la red
RoJoFloatMatrix cXb,cYb; //Guardarán coordenadas de mapa de background
RoJoFloatMatrix Vb; //Valor de cada pixel de mapa de background
RoJoFloatMatrix meanCost; //Promedio de los resultados de la función de coste
uint8_t resolution=20; //Número de rectángulos por eje para mapa de background
float error=0.01; //Resultado de la función de coste. Inicialmente quedará en modo espera.
uint16_t epoch=0; //Contador de ciclos

void setup() {
	Serial.begin(115200);
  //Configuración NeoPixel para M5Stack Fire. 5x2 en pin 15
  //Cada fila representa un lado: fila 0 = lado derecho, fila 1 = lado izquierdo
  //Las columnas representan el led.
  //En la derecha las columnas se cuentan de arriba a abajo
  //En el lado izquierdo se cuentan de abajo arriba.
  leds.begin(5,2,15);
	//Inicializamos el display en un M5Stack Fire
	//  pinRES_display=33;
	//  pinDC_display=27;
  //  pinCS_display=14;
  //  pinBackLight_display=32;
  display.begin(33,27,14,32);
	display.rotation(1);
	plt.begin(); //Init con dimensiones por defecto: 200*200
  //Matriz de valores correctos para el entrenamiento de la red
  //Tendrá el doble de muestras que la de un anillo y una sola columna:
  //el anillo al que pertenece
  Y.redim(samples*2,1);
}

void loop() {
	if(error>0.05) { //Mientras no tengamos un valor reducido de error...
    epoch++;
    if(epoch%25==0) { //Sólo cada cierto número de ciclos...
      //Calculamos el error tomando los valores de salida del último ciclo
      nn.cost(&Z,&Y,&meanCost);
      //Como sólo tenemos un parámetro de salida, la matriz meanCost
      //Sólo tendrá una columna (y una fila).
      //Tomaremos el valor como el error del estado actual
      error=meanCost.m[0][0]; //Lo anotamos para poder salir del bucle
			Serial.printf("ciclo:%d,error=%f\n",epoch,error);
      nn.mesh(&cXb,&cYb,&Vb); //Calculamos los valores del mapa de background
      //Dibujamos el mapa de background y pasamos como parámetros:
      //- cXb : la lista de coordenadas horizontales
      //- cYb : La lista de coordenadas verticales
      //- Vb : La matriz de valores
      //- meshRWB : La función que convierte los valores en colores
      //- autoLimit: debe calcular automáticamente los límites? NO
      plt.mesh(&cXb,&cYb,&Vb,meshRWB,false);
      plt.scatter(&R0,{255,0,0}); //Dibujamos anillo 0 en rojo
      plt.scatter(&R1); //Dibujamos anillo 1 en azul
			display.drawSprite(plt.spr,60,20); //Dibujamos el sprite de plt centrado en pantalla
    } else { //Si no toca mostrar resultados...
      nn.train(&X,&Y,&Z); //Entrenamos con normalidad
    }
	} else { //Si el cálculo ha terminado...
    if(error>0) { //Si se acaba de terminar el cálculo...
      error=0;
      leds.v->clear({0,50,0}); leds.draw(); //Leds en verde
    } else { //El cálculo se ha terminado hace más de dos ciclos...
      if(button1.pressed()) { //Si se ha pulsado el botón...
        //Inicializamos los datos
        //Objeto para generar anillos. Cuando termine el if, se eliminará
        RoJoMakeCircles circles; 
        circles.get(&R0,samples,0.5,0.1); //Generamos puntos de anillo 0 (interior,radio=0.5)
        circles.get(&R1,samples,1,0.1); //Generamos puntos de anillo 1 (exterior,radio=1)
        plt.axis(false,&R1); //Autocalcula escalas y dibuja ejes con datos de anillo exterior
        //Combinamos las coordenadas de ambos anillos en una sóla matriz para la red neuronal
        //Serán los valores de entrada para el entrenamiento de la red
        X.glueRows(&R0,&R1); //Pegamos las filas de ambos anillos en una sola matriz
        //Asignamos el valor correcto para cada anillo
        //Al unir las dos matrices en una se respeta el orden de los parámetros
        for(uint16_t i=0;i<samples;i++) {
          Y.m[i][0]=0; //Anillo 0 en la primera mitad
          Y.m[i+samples][0]=1; //Anillo 1 en la segunda mitad
        }
        //Desordenaremos los datos
        //Creamos una lista desordenada con tantos elementos como filas tenga la matriz Y
        //La lista contiene una secuencia que comienza en 0 y está desordenada
        uint16_t *sList=Y.scrambleList();
        //Desordenamos la matrices de coordenadas y valores teniendo en cuenta la lista
        X.scramble(sList);
        Y.scramble(sList);
        delete[] sList; //Ya no necesitamos la lista desordenada

        //Definimos el número de nodos de las capas
        //  2 : nodos de entrada a capa oculta 1
        //  4 : nodos entre capa oculta 1 y 2
        //  1 : nodos de salida de capa oculta 2
        uint16_t topology[]={2,4,1};
        //Inicializamos red neuronal con:
        //  2 : una capa oculta
        //  topology : número de nodos entre capas (entrada+ocultas+salida)
        //  1 : función de activación = sigmoide
        //  1 : función de coste = error cuadrático
        //  0.05 : learning rate
        nn.begin(2,topology,1,1,0.05);
        float xMin,yMin,xMax,yMax; //Guardarán los límites de visualización
        plt.getLimitsX(&xMin,&xMax); //Obtenemos límites horizontales
        plt.getLimitsY(&yMin,&yMax); //Obtenemos límites verticales
        cXb.linspace(xMin,xMax,resolution); //Calculamos coordenadas X de pixels de mapa de background
        cYb.linspace(yMin,yMax,resolution); //Calculamos coordenadas Y de pixels de mapa de background
        //Definimos los límites de los valores del mapa de background
        //Así no tendremos que calcularlo en cada ciclo
        //Realmente no es necesario, porque coinciden con los valores por defecto
        //[0,1] son los límites de la función sigmoide
        plt.meshMin=0;
        plt.meshMax=1;
        leds.v->clear({50,0,0}); leds.draw(); //Leds en rojo = trabajando
        error=1; //Error superior al límite mínimo provoca que comience a trabajar
        epoch=0; //Reseteamos el número de ciclos
      }
    }
  }
}
