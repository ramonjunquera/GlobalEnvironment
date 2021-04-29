/*
  Autor: Ramón Junquera
  Tema: Tarjeta SD
  Versión: 20180123
  Objetivo: Aprender a utilizar el adaptador de tarjetas micro SD
  Material: breadboard, cables, lector SD, Arduino Nano

  Descripción:
  Ejemplo simple de detección de tarjeta, tipo y tamaño
  
  Para este proyecto hemos tomado el ejemplo "CardInfo". Lo hemos modificado para que coincida en pines, hemos
  corregido algún error y hemos añadido comentarios.
  
  Resultado:
  Vemos los datos de la tarjeta y su contenido
*/

//Librería necesarias para conectar y gestionar el adaptador SD
#include <SD.h>

//Creamos los objetos que mantendrán la información de la SD en todo momento
Sd2Card card;
SdVolume volume;
SdFile root;

void setup()
{
  //Abrimos las comunicaciones por el puerto serie
  Serial.begin(115200);
  Serial.print("\nInicializando tarjeta SD...");

  //Si no se ha podido inicializar la tarjeta...
  //SPI_HALF_SPEED es un entero entre 1 y 6 que indica la velocidad de
  //las comunicaciones SPI
  //También se puede utilizar la función de inicialización añadiendo como parámetro
  //el pin de CS:
  //if (!card.init(SPI_HALF_SPEED, chipSelectPin))
  //Por defecto el pin en Arduino UNO/Nano es el 10, pero podría seleccionarse
  //cualquier otro
  if (!card.init(SPI_HALF_SPEED))
  {
    Serial.println("Fallo en la inicializacion. Comprobar:");
    Serial.println("* que hay una tarjeta metida");
    Serial.println("* que el adaptador esta correctamente conectado");
    Serial.println("* que el pin CS esta bien configurado");
    //Hemos terminado
    return;
  }
  else
  {
    //Hemos podido inicializar la tarjeta
    Serial.println("conexion correcta y tarjeta encontrada.");
  }

  //Mostramos el tipo de tarjeta
  Serial.print("\nTipo de tarjeta: ");
  switch (card.type())
  {
    case SD_CARD_TYPE_SD1:
      Serial.println("SD1");
      break;
    case SD_CARD_TYPE_SD2:
      Serial.println("SD2");
      break;
    case SD_CARD_TYPE_SDHC:
      Serial.println("SDHC");
      break;
    default:
      Serial.println("Desconocida");
  }

  //Si no podemos inicializar el volumen (partición)...
  if (!volume.init(card))
  {
    Serial.println("No se encontro una particion FAT16/FAT32.\nAsegurate de que la tarjeta esta formateada");
    //Hemos terminado
    return;
  }

  //Definimos la variable que contendrá el tamaño del volumen
  uint32_t volumesize;
  //Indicamos el tipo de FAT que es
  Serial.print("\nVolumen de tipo FAT");
  //Puede ser 16 o 32
  Serial.println(volume.fatType(), DEC);
  //Dejamos una línea en blanco
  Serial.println();

  //Una partición (volumen) FAT se divide en bloques.
  //Un bloque es la unidad mínima que ocupa un archivo
  //Los bloques están agrupados en clusters
  //El tamaño de un bloque en una tarjeta SD es siempre fijo, con independencia
  //del tamaño de la tarjeta. Siempre 512 bytes
  
  Serial.print("Bloques por cluster: ");
  Serial.println(volume.blocksPerCluster());
  Serial.print("Numero de clusters: ");
  Serial.println(volume.clusterCount());

  //El ejemplo CardInfo original fue desarrollado cuando las tarjetas SD no superaban los 4Gb
  //Hoy en día es muy habitual
  //El problema es que aun conserva un bug que impide que se muestre el tamaño correcto de la
  //tarjeta si es mayor de 4Gb.
  //El problema es que se ha definido la variable volumesize del tipo uint32_t (unsigned long)
  //que tiene un tamaño de 4 bytes.
  //Si calculamos cuántos bytes hay en 4Gb nos sale....
  //1024*1024*1024*4=2^10 * 2^10 * 2^10 * 2^2 = 2^32
  //Que es exáctamente el máximo valor que puede contener un unsigned long
  //Así que cuando intentamos calcular el tamaño de una tarjeta mayor de 4G en bytes, superamos el
  //límite máximo de la variable y obtenemos un valor erróneo.
  //La solución podría ser trivial... sustituimos el uint32_t (unsigned long) por el tipo
  //uint64_t (unsigned long long) que ocupa 8 bytes y puede almacenar hasta un valor de 2^64
  //El problema viene cuando intentamos mostrar el valor con un println()
  //El compilador nos da un error del tipo "error: call of overloaded 'println(uint64_t&)' is ambiguous"
  //porque no es capaz de imprimir el valor de este tipo de variables.
  //Es un error reconocido por Arduino.
  //Por lo tanto. Para solventar el problema lo que hacemos es NO calcular el tamaño de la tarjeta en bytes
  //Comenzamos calculándolo en Kb...
  
  //Tomamos nota del número de bloques por cluster
  volumesize=volume.blocksPerCluster();
  //Los multiplicamos por el número de clusters
  volumesize*=volume.clusterCount();
  //Ok. Tenemos en volumesize el número total de bloques de la tarjeta
  //Si ahora cometemos el "gran error" de multiplicarlo por 512 bytes/block y nuestra tarjeta tiene una capacidad
  //superior a las 4Gb, se desbordará obteniendo un resultado incorrecto (pero no dará error).
  //Una vez calculado ese valor, lo dividiríamos por 1024 para obtener el tamaño total en Kb.
  //Haremos esto es un sólo paso...
  //volumesize*512/1024 = volumesize/2
  
  //Mostramos el tamaño del volumen en Kb
  Serial.print("Tamano del volumen en Kb: ");
  volumesize/=2;
  Serial.println(volumesize);
  //Mostramos el volumen en Mb
  Serial.print("Tamano del volumen en Mb: ");
  //1024 Kb = 1 Mb
  volumesize/=1024;
  Serial.println(volumesize);
  //Mostramos el volumen en Gb
  Serial.print("Tamano del volumen en Gb: ");
  //1024 Mb = 1 Gb
  volumesize/=1024;
  Serial.println(volumesize);

  //Mostramos el listado de carpetas y archivos que contiene el volumen
  Serial.println("\nArchivos contenidos en la tarjeta (nombre, fecha, tamano (bytes): ");
  //Abrimos la carpeta raiz del volumen
  root.openRoot(volume);
  //Listamos todo el contenido del volumen (nombre), con fecha y tamaño
  root.ls(LS_R | LS_DATE | LS_SIZE);
}

void loop()
{
  //No tenemos nada especial que repetir
}
