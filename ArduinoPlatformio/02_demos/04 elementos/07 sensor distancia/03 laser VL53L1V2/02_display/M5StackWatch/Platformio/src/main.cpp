/*
  Autor: Ramón Junquera
  Tema: Sensor de distancia VL53L1X
  Fecha: 20210730
  Objetivo: Mostrar la distancia medida en un display
  Notas:
  - Suponemos que utilizamos un sensor con el identificador I2C por defecto
  Resultado:
    Se muestra la distancia medida en un display.
*/

#include <Arduino.h>
#include <RoJoSH1107.h> //Librería de gestión del display
#include <RoJoVL53L1X.h> //Librería de gestión del sensor LIDAR

//Declaración de constantes globales
const byte lidarSDA=13,lidarSCL=25; //Pinout de I2C en el puerto A del M5StackWatch

//Definición de objetos globales
RoJoSH1107 display;
RoJoSprite2 v(0); //Sprite monocromo para dibujar el texto
RoJoVL53L1X sensor; //Objeto de gestión del sensor LIDAR

void setup() {
  Serial.begin(115200);
  display.begin(33,27,14); //Inicializamos el display con los parámetros del M5Stack Watch
  if (!sensor.begin(lidarSDA,lidarSCL)) { //Si no podemos inicializar el sensor...
    Serial.println("Error inicializando el sensor"); //Informamos
    while(1); //Paramos
  }
  
  //Dimensionamos el sprite que usaremos como memoria de vídeo
  //Puesto que el sprite es monocromo, su altura debería se múltiplo de 8
  //Como usaremos una fuente de 23 pixels de altura, fijaremos la altura del sprite a 24
  //El texto tendrá un máximo de 4 dígitos. Si la anchura máxima de un dígito es de 15 y la separación
  //entre dígitos es de 1 pixel, tendremos una anchura máxima de: 15+1+15+1+15+1+15=15*4+3=63
  v.setSize(63,24);
}

void loop() {
  v.clear(); //Borramos el sprite
  //Leemos la distancia, la convertimos a String y la dibujamos en el sprite
  v.printOver("/15x23d.fon",String(sensor.read()),1);
  display.drawSprite(&v); //Dibujamos el sprite en pantalla
  delay(400); //Esperamos un poco para facilitar la lectura de los datos
}
