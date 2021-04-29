/*
  Autor: Ramón Junquera
  Fecha: 20201108
  Tema: Redes Neuronales
  Objetivo: Demo save/load

  Descripción:
	  Prueba de concepto de los métodos save & load del objeto RoJoNeuralNetwork.
		Se crea una red neuronal y se muestra su contenido.
		Se guarda. Se elimina. Se muestra su contenido para comprobar que está vacía.
		Se recupera y se vuelve a mostrar para poder comparar los datos con los
		originales, que deberían coincidir.

		Puesto que el objeto RoJoNeuralNetwork no tiene métodos para mostrar su
		contenido, creamos una clase derivada y se los añadimos.

		Nuestra red neuronal tendrá de nombre "/mynn", por lo tanto se utilzarán los
		siguientes archivos:
		- /mynn.nn : datos de la red neuronal
		- /mynn.0W : matrix W de la capa 0
		- /mynn.0b : matrix b de la capa 0
		- /mynn.1W : matrix W de la capa 1
		- /mynn.1b : matrix b de la capa 1

	Nota:
	  En el ejemplo sólo se utilizan funciones tipificadas, tanto para la de activación
		como para la de coste.
		En el caso de que haya funciones personalizadas, una vez se haya completado la 
		recuperación (load), se deberán asignar de nuevo manualmente.
*/

#include <Arduino.h>
#include <RoJoNeoPixel.h> //Gestión de leds NeoPixel
#include <RoJoFloatMatrix.h> //Gestión de matrices
#include <RoJoNeuralNetwork.h> //Redes neuronales

//Creamos una clase nueva derivada de RoJoNeuralNetwork
//Sólo le añadimos métodos para mostrar contenido
class RoJoNeuralNetworkShow:public RoJoNeuralNetwork
{
  public:
	  void showMatrix(RoJoFloatMatrix *M) {
			for(uint16_t row=0;row<M->rows();row++) {
				for(uint16_t col=0;col<M->cols();col++) {
					Serial.printf("%f\t",M->m[row][col]);
				}
				Serial.println();
			}
		}
	  void show() {
			Serial.println();
			Serial.printf("Número de capas: %i\n",_layersCount);
			Serial.printf("Índice de función de coste: %i\n",_indexCostF);
			Serial.printf("Learning Rate: %f\n",_lr);
			for(uint16_t layer=0;layer<_layersCount;layer++) {
				Serial.printf("Layer: %i\n",layer);
				Serial.printf("Índice de función de activación: %i\n",_layers[layer]->_indexActF);
				Serial.println("Matriz W");
				showMatrix(&(_layers[layer]->_W));
				Serial.println("Matriz b");
				showMatrix(&(_layers[layer]->_b));
			}
		}
}; //Punto y coma obligatorio para que no de error

//Objetos globales
RoJoNeoPixel leds; //Objeto de gestión de leds NeoPixel
RoJoNeuralNetworkShow nn; //Instanciamos la clase derivada

void setup() {
	Serial.begin(115200);
	//Configuración NeoPixel para M5Stack Fire. 5x2 en pin 15
  //Cada fila representa un lado: fila 0 = lado derecho, fila 1 = lado izquierdo
  //Las columnas representan el led.
  //En la derecha las columnas se cuentan de arriba a abajo
  //En el lado izquierdo se cuentan de abajo arriba.
  leds.begin(5,2,15);
	leds.v->clear({10,0,0}); leds.draw(); //Leds en rojo

	//Definimos topología: 2 capas ocultas
	//  2 : nodos de entrada
	//  4 : nodos cpara oculta 1 & 2
	//  1 : nodos de salida
	uint16_t topology[]={2,4,1};
	//Inicializamos red neuronal con:
	//  2 : dos capas ocultas
	//  topology : número de nodos entre capas (capas ocultas +1)
	//  1 : función de activación = sigmoide
	//  1 : función de coste = error cuadrático
	//  0.05 : learning rate
	nn.begin(2,topology,2,1,0.05);
	//Actualmetne todas las capas tienen la función de activación 2 (ReLU).
	//Asignaremos la función 1 (Sigmoidea) a la capa 1
	nn.setActF2Layer(1,1);
	nn.show();
	if(nn.save("/mynn")) Serial.println("save ok");
	else Serial.println("save KO");

	nn.end();
	Serial.println("red neuronal borrada");
	nn.show();

	if(nn.load("/mynn")) Serial.println("load ok");
	else Serial.println("load KO");
	nn.show();
}

void loop() {
  
}
