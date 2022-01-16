/*
  Tema: Almacenamiento de constantes en memoria
  Versión: 20211221
  Autor: Ramón Junquera

  Problema:
  Algunas de las placas que utilizan en entorno de desarrollo de Arduino tienen una cantidad de
  memoria muy limitada. Por ejemplo Arduino UNO o Nano disponen de 32Kb.
  Esta memoria debe ser compartida entre:
  - El código del programa compilado
  - El valor de variables y constantes definidas
  - Las variables creadas en tiempo de ejecución

  El programa se guarda comenzando desde el principio de la memoria.
  El resto alberga la memoria de datos.
  El problema es que la memoria de datos suele estar limitada. No se puede usar toda la memoria
  que no usa el programa para datos.
  En Arduino UNO o Nano, la memoria de datos está limitada a 2Kb. Además cuando se utiliza más de
  1.5Kb comenzamos a tener problemas de estabilidad.

  Tenemos varias soluciones:
  - Utilizamos la memoria EEPROM. Recordemos que en Arduino UNO o Nano tenemos 1Kb de EEPROM que
    se mantiene incluso sin alimentación. El problema es que deberíamos prepararla antes y el
    número de ciclos de lectura-escritura es bastante limitado.
  - Utilizamos una tarjeta SD. El acceso es mucho más lento, pero aumentamos el límite de 
    almacemaniento de 2Kb hasta 64Gb.
  - Utilizamos la memoria de programa para almacenar los datos.

  Esta última opción es la más utilizada, por comodidad.
  La memoria de programa contendrá la información de las constantes, ocupando más, pero dejando
  la misma memoria de variables que antes.
  Simplemente mejoramos la eficiencia de uso de la memoria.

  Se puede forzar al compilador a que almacene las esctructuras de datos en la memoria de programa
  incluyendo el comando PROGMEM junto con la definición de los datos.

  El funcionamiento de este comando tiene distinto comportamiento dependiendo de la placa en la
  que se aplica.

  Por otra parte, hay que tener en cuenta que los datos almacenados en memoria de programas no 
  pueden ser accedidos directamente. Siempre seben ser transferidos primero a la memoria de datos
  y una vez allí se podrán procesar.
  Si no utilizamos el comando PROGMEM, por defecto los datos quedan almacenados tanto en memoria
  de programa como en memoria de datos para ser utilizados.

  Los datos definidos en el programa principal no tienen porqué ser tratados de la misma manera
  que los definidos en archivos externos (includes).

  Este problema sólo afecta a dispositivos Arduino. La familia ESP (ESP8266 & ESP32) no lo necesita,
  porque ya guarda las constantes en memoria de programa, además de disponer de bastqante más memoria
  para datos.
  
  Para su recuperación utilizaremos la colección de instrucciones pgm_read_*_near para leer los valores.

  Demo:
  Se declaran varios tipos de constantes:
  - Un array de bytes
  - Un array de uint16_t
  - Un array de caracteres que se recuperará carácter a carácter en un String
  - Un array de caracteres que se recuperará de una sola vez copiando su contenido a un
    buffer en la memoria de variables
  - Un texto que se declara en el lugar de uso con la función F()
*/

#include <Arduino.h>

const PROGMEM byte byte1[] = {11,22,33};
const PROGMEM uint16_t byte2[] = {1111,2222,3333};
const PROGMEM char string1[] = "Esta es la primera prueba";
const PROGMEM char string2[] = "Esta es la segunda prueba";

void setup() {
  Serial.begin(115200);
  while (!Serial); //Esperamos a que el puerto serie esté disponible

  //Mostramos todos los valores del array de byte
  for(byte i=0;i<3;i++) {
    byte value=pgm_read_byte_near(&byte1[i]);
    Serial.println(value);
  }
  
  //Mostramos todos los valores del array de uint16_t
  for(byte i=0;i<3;i++) {
    uint16_t value=pgm_read_word_near(&byte2[i]);
    Serial.println(value);
  }

  String s=""; //Cadena inicial vacía
  byte len=strlen_P(string1); //Obtenemos la longitud de la cadena
  //Añadimos cada uno de los caracteres de la cadena original
  for(byte i=0;i<len;i++) s+=(char)pgm_read_byte_near(string1+i);
  Serial.println(s);

  char buffer[30]; //buffer de recuperación. Siempre mayor que la longitud de la cadena
  strcpy_P(buffer,string2); //Copiamos al buffer la cadena original incluido el byte 0 de fin de cadena
  Serial.println(buffer);

  //El texto constante se guarda directamente en la memoria de programas con la función F()
  Serial.println(F("Esta es la tercera prueba"));
}

void loop() {
}
