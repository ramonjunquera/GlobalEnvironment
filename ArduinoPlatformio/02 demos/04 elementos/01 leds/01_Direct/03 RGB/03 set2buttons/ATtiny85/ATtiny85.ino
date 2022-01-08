/*
  Autor: Ramón Junquera
  Fecha: 20191027
  Tema: Lectura de estado de un pin
  Objetivo: Uso de todos los pines para un proyecto de lectur/escritura
  Material adicional: ATtiny85, breadboard, 2 pulsadores, led RGB, resistencia de 200 ohmios

  Descripción:
  Crearemos un proyecto que mezcla lecturas y escrituras.
  Utilizaremos los 5 pines disponibles.
  Tendremos dos pulsadores conectados a los dos primeros pines (0,1)
  Y un led RGB conectado a los 3 últimos pines (2 ,3 y 4).

  Su no pulsamos nada, no se encenderá ninguna luz.
  Si pulsamos el primero, se verá el color rojo.
  Si pulsamos el segundo, se verá el color verde.
  Y si pulsamos ambos se verá el color azul.
  Siempre se verá un sólo color. Nunca se encenderá más de un led a la vez.

  Configuramos los pines de los interruptores como entradas con las resistencias
  internas de pullup.
  Configuramos os pines de los leds como salidas.

  Creamos 3 variables booleanas que corresponderán con el estado de cada uno de los leds.
  Inicialmente todos están apagados.

  Leemos los estados de los interruptores y decidimos qué led se debe encender.

  Finalmente aplicamos los estados de los leds.

  Puesto que sabemos de antemano que sólo habrá un máximo de un led encendido al
  mismo tiempo, podemos utilizar una única resistencia de 220 ohmios en el ánodo
  del led RGB.
  En el caso de que en algún momento tuviésemos que encender más de un led
  simultáneamente, tendríamos que usar 3 resistencias, una para cada led, colocadas
  en los cátodos.

  Puesto que el led RGB es de ánodo común, al aplicar el estado HIGH es apaga. Al revés
  de lo que indica la lógica.
  Por esa razón se cambia el estado antes de aplicarlo.

  Nota:
  Este proyecto NO funciona cuando suministramos la alimentación a través del USB
  porque interfiere en el estado de los pines.
  Se debe alimentar con una fuente externa a través de los pines GND & 5V

  Resultado:
  Con la combinacion de ambos interruptores, podemos encender cada uno de los 3 colores
  del led RGB
*/

void setup() { 
  //Configuramos los 2 primeros pines como entrada para los pulsadores
  pinMode(0,INPUT_PULLUP);
  pinMode(1,INPUT_PULLUP);
  //Configuramos los pines del led RGB como salidas
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
}

void loop() {
  //Definimos las variables que indicarán qué color está encendido
  bool R,G,B;
  //Inicialmente todos los colores están apagados
  R=G=B=false;
  //Si el primer interruptor está pulsado...encenderemos el color rojo
  if(!(digitalRead(0))) R=true;
  //Si el segundo interruptor está pulsado...
  if(!(digitalRead(1))) {
    //Si ya teníamos encendido el color rojo porque se había pulsado el primer interruptor...
    if(R) {
      //Apagamos el rojo
      R=false,
      //Y encendemos el azul
      B=true;
    } else { //Si sólo está pulsado el segundo interruptor
      //Encenderemos el color verde
      G=true;
    }
  }
  //Aplicamos los distintos estados a los leds
  digitalWrite(2,!R);
  digitalWrite(3,!B);
  digitalWrite(4,!G);
}
