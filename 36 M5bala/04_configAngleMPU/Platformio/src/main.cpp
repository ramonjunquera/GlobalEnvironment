/*
  Autor: Ramón Junquera
  Fecha: 20201213
  Tema: Librería para M5Bala
  Objetivo: Obtención de valores de offset de ángulo
  Material: Módulo M5Bala, M5Stack Fire

  Descripción:
	  Al desarrollar cualquier aplicación para el módulo Bala siempre tendremos que tener en
		cuenta el acelerómetro y el giróscopo integrados en M5Stack Fire.
		En teoría vienen calibrados. En la práctica no es así.
		El programa actual facilita la obtención de los valores de offset de ángulo para
		calibrarlos correctamente.
		Se recopilan muestras de los 3 ejes tanto para el acelerómetro como para el giróscopo.
		Mientras se recopilan se muestra una barra de progreso para indicar cuánto tiempo
		falta para finalizar el grupo de muestras.
		Puesto que el giróscopo nos devuelve la velocidad angular, sólo debemos preocuparnos
		de que el robot esté inmóvil. No nos importa la posición.
		Para el acelerómetro necesitaremos que además de estar quieto, se encuentre en equilibrio.
		Con el primer eje calcularemos la horizontalidad de las ruedas. Sólo necesitamos que ambas
		ruedas se mantengan en el mismo plano horizontal. Este eje no es muy importante para guardar
		el equilibrio.
		El segundo eje indica la horizontalidad hacia adelante y atrás. Es el eje más importante
		para guardar el equilibrio. Intentaremos apoyar el robot contra algo, pero procurando que
		haga la mínima fuerza. Debería estar muy cerca del punto de equilibrio.
		Este punto variará con cada robot, simplemente por la posición de los cables.
		Cada uno tendrá el centro de gravedad en un lugar distinto.
		El tercer debería ser cero cuando el robot está caido. Apoyado lateralmente en el suelo.
		Este offset no tiene demasiada importancia. Sólo nos servirá para saber si el robot ha
		caido.
		Por lo tanto, no se pueden obtener los 3 offsets del acelerómetro simultáneamente.
		Es conveniente obtener los dos primeros manteniendo el robot estático cerca de su punto de
		equilibrio en un plano horizontal.
		Después lo tumbaremos y anotaremos el offset del tercer eje del acelerómetro.

		Tal y como están configurados el acelerómetro y el giróscopo, el acelerómetro es más
		sensible que el giróscopo.
		Una vez obtenidos los offsets, podemos aplicarlos para comprobar si dan resultado.
		Esto lo hacemos con las dos últimas líneas comentadas de la función setup().

		En cualquier caso hay que ener en cuenta también el periodo.
		Por lo tanto queda como un ejercicio, pero siempre se tedrán que hacer la mediciones
		de offset en el programa que corresponda.
*/

#include <Arduino.h>
#include <RoJoILI9341.h> //Librería de gestión del display ILI9341
#include <RoJoMPU9250.h> //Gestión de MPU

//Constantes globales
const byte items=50;

//Objetos globales
RoJoILI9341 display; //Objeto de gestión de display
RoJoMPU9250 gam; //Gestión de MPU

struct status {
	float A[3]; //Ángulo de de acelerómetro
	float G[3]; //Ángulo de giróscopo
};

//Variables globales
status data[items]; //Array con datos recopilados

void setup() {
  //Inicializamos el display en un M5Stack Fire
  //  pinRES_display=33;
  //  pinDC_display=27;
  //  pinCS_display=14;
  //  pinBackLight_display=32;
  display.begin(33,27,14,32);
	display.rotation(1); //Botones abajo
	gam.begin(); //Inicialización de MPU
	gam.sleepM(); //No necesitaremos el magnetómetro
	gam.resetGA(1,0); //dps=1=500 grados por segundo,g=0=2G

	//Ejemplo de aplicación de offset para la calibración
  //gam.offsetAngleA[0]=1.05; gam.offsetAngleA[1]=-1.27; gam.offsetAngleA[2]=0.26;
  //gam.offsetAngleG[0]=1.15; gam.offsetAngleG[1]=-0.95; gam.offsetAngleG[2]=0.94;
}

void loop() {
	display.clear();
	display.rect(0,0,319,30,{0,255,0}); //Rectángulo de barra de progreso en verde
	float Amed[3]={0,0,0},Gmed[3]={0,0,0}; //Valores medios
	//Calculamos la suma de todos los valores
	for(byte m=0;m<items;m++) {
		for(byte i=0;i<3;i++) {
			Amed[i]+=data[m].A[i];
			Gmed[i]+=data[m].G[i];
		}
	}
	//Calculamos el promedio y le cambiamos el signo porque es un ajuste (offset)
	for(byte i=0;i<3;i++) {
		Amed[i]/=-items;
		Gmed[i]/=-items;
	}
	//Los mostramos
	display.printOver("/10x22.fon","A: "+String(Amed[0])+" , "+String(Amed[1])+" , "+String(Amed[2]),{0,255,0},0,40);
	display.printOver("/10x22.fon","G: "+String(Gmed[0])+" , "+String(Gmed[1])+" , "+String(Gmed[2]),{0,255,0},0,60);
  //Recopilamos una nueva colección de valores
	uint16_t barXLast=1; //Ánterior coordenada dibujada en barra de progreso
	uint16_t barXNow; //Actual coordenada dibujada en barra de progreso
	for(byte i=0;i<items;i++) { //Recogemos muestras...
	  gam.angleA(data[i].A); //Leemos acelerómetro
		gam.angleG(data[i].G); //Leemos giróscopo
    barXNow=(i*316)/items+2; //Calculamos hasta donde llega ahora la barra de progreso
		display.block(barXLast+1,2,barXNow,28,{200,100,0}); //Dibujamos lo que le falta
		barXLast=barXNow; //La próxima vez seguiremos desde aquí
		delay(100); //Esperamos 0.1s = 10 muestras por segundo
	}
}
