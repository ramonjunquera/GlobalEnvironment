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
    El carácter de final de línea es \r
    La clave se separa de su valor por el carácter ;
    La información se guarda en un archivo. Nunca en memoria.
*/


#ifndef RoJoFileDictionary_cpp
#define RoJoFileDictionary_cpp

#include <RoJoFileDictionary.h>

//Inicialización
//Devuelve true si consigue inicializar sin errores
bool RoJoFileDictionary::begin(String filename) {
  //Creamos este método porque en ESP32 no se puede inicializar SPIFFS
  //dentro de un constructor. Da un error en tiempo de ejecución

  //Si ya teníamos inicializado el diccionario...terminamos con error
  if(_filename.length()>0) return false;

  #ifdef ROJO_PIN_CS_SD //Si utilizamos SD...
    SD.begin(ROJO_PIN_CS_SD); //Inicializamos sistema de archivos
    if(!SD.exists(filename)) //Si el archivo no existe...
    {
      File f=SD.open(filename,FILE_WRITE); //Creamos un archivo nuevo
      if(!f) return false; //Si no hemos podido...terminamos con error
      f.close(); //Cerramos el archivo de escritura
    }
    //Ahora seguro que el archivo existe
    File f=SD.open(filename,FILE_READ); //Abrimos el archivo como lectura
  #else //Si utilizamos SPIFFS...
    SPIFFS.begin(); //Inicializamos sistema de archivos
    if(!SPIFFS.exists(filename)) { //Si el archivo no existe...
      File f=SPIFFS.open(filename,"w"); //Creamos un archivo nuevo
      if(!f) return false; //Si no hemos podido...terminamos con error
      f.close(); //Cerramos el archivo de escritura
    }
    //Ahora seguro que el archivo existe
    File f=SPIFFS.open(filename,"r"); //Abrimos el archivo como lectura
  #endif
  if(!f) return false; //Si no se pudo abrir...terminamos con error
  //Tenemos el archivo abierto en modo lectura

  //Mientras el archivo tenga algo que leer...
  while(f.available()>0) {
    //Leemos la línea completa y no la guardamos
    f.readStringUntil('\r');
    //Aumentamos el contador de registros
    _count++;
  }
  //Cerramos el archivo
  f.close();
  //Anotamos el nombre del archivo que guarda el diccionario
  _filename=filename;
  //Todo Ok
  return true;
}

//Devuelve el número de items en el diccionario
uint16_t RoJoFileDictionary::count() {
  return _count;
}

//Indica si existe la clave indicada
bool RoJoFileDictionary::containsKey(String key) {
  //Si el diccionario no se ha inicializado...la clave nunca existirá
  if(_filename.length()==0) return false;

  #ifdef ROJO_PIN_CS_SD //Si utilizamos SD...
    File f=SD.open(_filename,FILE_READ); //Abrimos el archivo en la SD
  #else //Si utilizamos SPIFFS...
    File f=SPIFFS.open(_filename,"r"); //Abrimos el archivo en SPIFFS
  #endif
  //Mientras el archivo tenga algo que leer...
  while(f.available()>0) {
    //...leemos la clave
    String keyRead=f.readStringUntil(';');
    //Si la clave coincide con la que buscamos...
    if(keyRead==key) {
      //...cerramos el archivo
      f.close();
      //Indicamos que la clave existe
      return true;
    }
    //Leemos el resto de línea
    keyRead=f.readStringUntil('\r');
  }
  //Hemos terminado de leer el archivo. Lo cerramos
  f.close();
  //No hemos encontrado la clave buscada
  return false;
}

//Elimina una clave
//Devuelve true si la clave existía
bool RoJoFileDictionary::remove(String key) {
  //Si el diccionario no se ha inicializado...la clave nunca ha existido
  if(_filename.length()==0) return false;

  //Hemos encontrado la clave buscada?. Inicialmente no
  bool keyFound=false;
  
  //Necesitamos crear un nombre para el archivo temporal
  //Serán 8 dígitos aleatorios con extensión tmp
  String tempFile="/";
  for(byte i=0;i<8;i++) tempFile+=(char)random('0','0'+10);
  tempFile+=".tmp";

  #ifdef ROJO_PIN_CS_SD //Si utilizamos SD...
    File f=SD.open(_filename,FILE_READ); //Abrimos el archivo como lectura
    File t=SD.open(tempFile,FILE_WRITE); //Abrimos el archivo temporal como escritura
  #else //Si utilizamos SPIFFS...
    File f=SPIFFS.open(_filename,"r"); //Abrimos el archivo como lectura
    File t=SPIFFS.open(tempFile,"w"); //Abrimos el archivo temporal como escritura
  #endif
  
  while(f.available()>0) { //Mientras el archivo tenga algo que leer...
    String keyRead=f.readStringUntil(';'); //...leemos la clave
    String valueRead=f.readStringUntil('\r'); //Leemos el valor
    //Si la clave coincide con la que buscamos...anotamos que hemos encontrado la clave
    if(keyRead==key) keyFound=true;
    //Si no es la clave que buscamos...escribimos el registro en el archivo temporal
    else t.print(keyRead+";"+valueRead+"\r");
  }
  //Hemos terminado de leer el archivo.
  //Cerramos tanto el archivo temporal como el de diccionario
  t.close();
  f.close();

  #ifdef ROJO_PIN_CS_SD //Si utilizamos SD...
    if(keyFound) { //Si hemos encontrado la clave buscada...
      SD.remove(_filename); //...borramos el archivo de diccionario
      #ifdef ARDUINO_ARCH_AVR //Si es un Arduino...
        //La librería de gestión de SD de Arduino NO tiene el
        //método rename. Tenemos que crear el archivo, copiar
        //su contenido y borrar el original :-(
        File t=SD.open(tempFile,FILE_READ); //Abrimos el archivo temporal como lectura
        File f=SD.open(_filename,FILE_WRITE); //Abrimos el archivo de diccionarion como escritura
        while(t.available()>0) f.write((byte)t.read());
        f.close();
        t.close();
        SD.remove(tempFile);
      #else //Si no es un Arduino...
        SD.rename(tempFile,_filename); //Renombramos el temporal al de diccionario
      #endif
      _count--; //Reducimos el número de registros
    }
    //Si no hemos encontrado la clave buscada...borramos el archivo temporal
    else SD.remove(tempFile);
  #else //Si utilizamos SPIFFS...
    if(keyFound) { //Si hemos encontrado la clave buscada...
      SPIFFS.remove(_filename); //...borramos el archivo de diccionario
      SPIFFS.rename(tempFile,_filename); //Renombramos el temporal al de diccionario
      _count--; //Reducimos el número de registros
    }
    //Si no hemos encontrado la clave buscada...borramos el archivo temporal
    else SPIFFS.remove(tempFile);
  #endif
  //Devolvemos si la clave existía
  return keyFound;
}

//Borra el contenido del diccionario
//Devuelve true si lo consigue
bool RoJoFileDictionary::clear() {
  //Si el diccionario no se ha inicializado...no se puede vaciar
  if(_filename.length()==0) return false;

  #ifdef ROJO_PIN_CS_SD //Si utilizamos SD...
    if(SD.remove(_filename)) { //Si podemos borrar el archivo de diccionario...
      File f=SD.open(_filename,FILE_WRITE); //Lo creamos de nuevo
      if(!f) return false; //Si no se ha podido crear el archivo...devolvemos error
      f.close();
    }
  #else //Si utilizamos SPIFFS...
    if(SPIFFS.remove(_filename)) { //Si podemos borrar el archivo de diccionario...
      File f=SPIFFS.open(_filename,"w"); //Lo creamos de nuevo
      if(!f) return false; //Si no se ha podido crear el archivo...devolvemos error
      f.close();
    }
  #endif
  //Todo Ok
  _count=0;
  return true;
}

//Añade un item al diccionario
//Si ya existe lo actualiza
//Devuelve true si ya existía la clave
bool RoJoFileDictionary::add(String key,String value) {
  //Si el diccionario no se ha inicializado...clave no existía
  if(_filename.length()==0) return false;

  //Borramos la clave si es que existe
  //Y anotamos si ya existía
  bool alreadyExists=remove(key);
  //Añadimos la nueva
  //Abrimos el archivo como append
  #ifdef ROJO_PIN_CS_SD //Si utilizamos SD...
    #ifdef ARDUINO_ARCH_AVR //Si es un Arduino... 
      //Con este driver, los archivos de escritura
      //ya se abren en modo append
      File f=SD.open(_filename,FILE_WRITE);
    #else //Si no es un Arduino...
      File f=SD.open(_filename,FILE_APPEND);
    #endif
  #else //Si utilizamos SPIFFS...
    File f=SPIFFS.open(_filename,"a");
  #endif
  //Escribimos el nuevo registro
  f.print(key+";"+value+"\r");
  //Cerramo archivo
  f.close();
  //Tenemos un nuevo registro
  _count++;
  //Respondemos si ya existía la clave
  return alreadyExists;
}

//Añade un item al diccionario
//Si ya existe lo actualiza
//Devuelve true si ya existía la clave
bool RoJoFileDictionary::add(RoJoFileDictionaryItem item) {
  return add(item.key,item.value);
}

//Obtiene el valor de una clave
//Si no existe devuelve el valor por defecto
String RoJoFileDictionary::value(String key,String defaultValue) {
  //Si el diccionario no se ha inicializado...el valor no existe
  if(_filename.length()==0) return defaultValue;

  String keyRead;
  String valueRead;

  #ifdef ROJO_PIN_CS_SD //Si utilizamos SD...
    File f=SD.open(_filename,FILE_READ);
  #else //Si utilizamos SPIFFS...
    File f=SPIFFS.open(_filename,"r");
  #endif
  
  while(f.available()>0)
  {
    keyRead=f.readStringUntil(';');
    valueRead=f.readStringUntil('\r');
    if(keyRead==key) {
      f.close();
      return valueRead;
    }
  }
  f.close();
  return defaultValue;
}

//Obtiene el item de una posición
//Si el índice está fuera de rango devuelve un item con clave y valor vacíos
RoJoFileDictionaryItem RoJoFileDictionary::item(uint16_t index) {
  //Creamos el item a devolver
  RoJoFileDictionaryItem a;
  //Si el diccionario se ha inicializado y el índice es válido...
  if(_filename.length()>0 && index<_count) {
    #ifdef ROJO_PIN_CS_SD //Si utilizamos SD...
      File f=SD.open(_filename,FILE_READ);
    #else //Si utilizamos SPIFFS...
      File f=SPIFFS.open(_filename,"r");
    #endif

    if(f) { //Si hemos podido abrir el archivo...
      for(uint16_t i=0;i<=index;i++) {
        a.key=f.readStringUntil(';');
        a.value=f.readStringUntil('\r');
      }
      f.close();
    }
  }
  return a;
}

//Obtiene la clave de una posición
//Si el índice está fuera de rango devuelve una clave vacía
String RoJoFileDictionary::key(uint16_t index) {
  return item(index).key;
}

//Obtiene el valor de una posición
//Si el índice está fuera de rango devuelve un valor vacío
String RoJoFileDictionary::value(uint16_t index) {
  return item(index).value;
}

//Finaliza el uso del diccionario
void RoJoFileDictionary::end() {
  //Ya no tenemos items
  _count=0;
  //Ni tenemos nombre de archivo
  _filename="";
}

#endif
