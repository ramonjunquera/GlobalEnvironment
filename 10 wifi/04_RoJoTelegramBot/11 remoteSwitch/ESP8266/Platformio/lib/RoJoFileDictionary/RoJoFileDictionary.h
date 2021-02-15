/*
  Nombre de la librería: RoJoFileDictionary.h
  Versión: 20190910
  Autor: Ramón Junquera
  Descripción:
    El sistema de archivos donde se guardan y leen los sprites es seleccionable
    mediante la constante global del preprocesador ROJO_PIN_CS_SD
    Estas constantes se definen en el archivo platformio.ini.

    Selección del sistema de archivos.
      Si se declara la constante ROJO_PIN_CS_SD y se le asigna un valor, se
      selecciona la SD como sistema de archivos.
      El valor de ROJO_PIN_CS_SD corresponde con el pin CS de la SD.
      Si no se declara esta constante, se utilizará SPIFFS.
      Ej.: build_flags = -D ROJO_PIN_CS_SD=15

    Simulación de clase diccionario basado en sistema de archivos.
    Guarda pares de valores (registros) en un archivo de texto.
    Un registro consta de clave (key) y valor asociado (value).
    Las claves son únicas.
    Cada registro se almacena en una línea.
    La clave se separa de su valor por el carácter ;
    La información se guarda en un archivo. Nunca en memoria.
*/

#ifndef RoJoFileDictionary_h
#define RoJoFileDictionary_h

#include <Arduino.h>
#ifdef ROJO_PIN_CS_SD //Si debemos utilizar SD...
  #ifdef __arm__ //Si es una Raspberry...
    #error Raspberry has not SD
  #endif
  //No es una Raspberry
  //Puede tener SD. Lo cargamos
  #include <SD.h> //Librería de gestión de SD
#else //Si debemos utilizar SPIFFS...
  #ifdef ARDUINO_ARCH_AVR //Si es una placa Arduino...
    #error Arduino family has not SPIFFS
  #endif
  //No es una placa Arduino. Puede tener SPIFFS
  //Cargamos la librería en función del dispositivo
  #ifdef ESP32 //Si es un ESP32...
    #include <SPIFFS.h> //Librería de gestión SPIFFS para ESP32
  #else //Si cualquier otra cosa (ESP8266 o RPi)...
    #include <FS.h> //Librería de gestión SPIFFS para ESP8266
  #endif
#endif

struct RoJoFileDictionaryItem {
  String key="";
  String value="";
};

class RoJoFileDictionary {
  private:
    String _filename; //Nombre del archivo que guarda el diccionario
    uint16_t _count=0; //Número de registros
  public:
    bool begin(String fileName); //Inicialización
    uint16_t count(); //Número de items en el diccionario
    bool containsKey(String key); //Contiene la clave indicada?
    bool remove(String key); //Elimina una clave
    bool clear(); //Borra el contenido del diccionario
    bool add(String key,String value); //Añade un item al diccionario
    bool add(RoJoFileDictionaryItem item); //Añade un item al diccionario
    String key(uint16_t index); //Obtiene la clave de una posición
    String value(uint16_t index); //Obtiene el valor de una posición
    String value(String key,String defaultValue); //Obtiene el valor de una clave
    RoJoFileDictionaryItem item(uint16_t index); //Obtiene el item de una posición
    void end(); //Finaliza el uso del diccionario
}; //Punto y coma obligatorio para que no de error

#ifdef __arm__
  #include <RoJoFileDictionary.cpp> //Para guardar compatibilidad con RPi
#endif

#endif