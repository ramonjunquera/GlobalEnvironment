/*
  Autor: Ramón Junquera
  Tema: Sensor de distancia VL53L1X
  Fecha: 20210730
  Objetivo: Mostrar la distancia medida en un display
  Notas:
  - Suponemos que utilizamos un sensor con el identificador I2C por defecto (0x29).
  Resultado:
    Se muestra la distancia medida en un display.
*/

#include <Arduino.h>
#include <RoJoSSD1306.h> //Librería de gestión del display SSD1306
#include <RoJoVL53L1X.h> //Librería de gestión del sensor LIDAR

//Declaración de constantes globales
const byte I2CSDA=4,I2CSCL=15; //Pinout de I2C
const byte displayRES=16; //Pin de reset de display integrado

//Declaración de variables globales
RoJoSSD1306 display; //Objeto de gestión de display
RoJoSprite2 v(0); //Sprite monocromo para dibujar el texto
RoJoVL53L1X sensor; //Objeto de gestión del sensor LIDAR

void setup() {
  Serial.begin(115200);
  display.begin(I2CSDA,I2CSCL,displayRES); //Init display
  //Nota:
  //La inicialización del display incluye la inicialización de I2C en los pines indicados
  //y una frecuencia de 700KHz.
  //Según las especificaciones del sensor LIDAR, sólo soporta un máximo de 400KHz.
  //En la práctica no parece cierto, puesto que tambnién soporta los 700KHz
  //De todas maneras la librería RoJoVL53L1X fija la frecuencia a los 400KHz oficiales
  //Por eso primero inicializamos el display y después el sensor
  //Si no podemos inicializar el sensor con el identificador I2C por defecto y en los
  //pines indicados...
  if (!sensor.begin(I2CSDA,I2CSCL)) {
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
  //Dibujamos el sprite en pantalla
  //Centrado horizontal (más o menos):
  //  El márgen izquierdo es de (128-63)/2=32
  //Centrado vertical:
  //  Puesto que la coordenada vertical debe ser múltiplo de 8, el sprite tiene 3 páginas de altura
  //  y el display 8, mantendremos un márgen vertical de 2 páginas (16 pixels)
  display.drawSprite(&v,32,16);
  delay(400); //Esperamos un poco para facilitar la lectura de los datos
}