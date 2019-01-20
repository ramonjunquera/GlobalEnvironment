/*
  Autor: Ramón Junquera
  Tema: ArduCAM
  Versión: 20180311
  Objetivo: Demo de librería RoJoArduCAM
  Material adicional: placa ESP, ArduCAM-Mini-2MP

  Descripción:
  Crearemos una aplicación para mostrar el uso de la librería RoJoArduCAM.
  Mediante el envío de comando por el puerto serie podremos seleccionar el formato y la resolución
  utilizadas para capturar imágenes.
  Los comandos son los siguientes:
    0: Cambia la resolución a 160x120
    1: Cambia la resolución a 176x144
    2: Cambia la resolución a 320x230
    3: Cambia la resolución a 352x288
    4: Cambia la resolución a 640x480
    5: Cambia la resolución a 800x600
    6: Cambia la resolución a 1024x768
    7: Cambia la resolución a 1280x1024
    8: Cambia la resolución a 1600x1200
    A: Inicia la captura de una foto
    B: Comprueba si ha finalizado la captura
    C: Guarda la foto en sistema de archivos
    D: Cambia a formato bmp
    E: Cambia a formato jpg
    H: Ayuda

  Los archivos de imagen se almacenarán en el sistema de archivos.
  Para recuperarlos necesitaremos herramientas como el servidor FTP que nos permite gestionar el
  contenido del sistema de archivos.

  Es imprescindible que el archivo RoJoArduCAM.dat se encuentre en el sistema de archivos.

  Si la cámara se inicializa con algún error, se mostrará el código por el puerto serie y 
  el led integrado parpadeará tantas veces como el código de error.
  Los códigos de error de la inicialización son:
    0: No hubo errores
    1: Error al inicializar el sistema de archivos
    2: Falta el archivo RoJoArduCAM.dat
    3: Error de conexión SPI
    4: La cámara no es el modelo esperado

  Listado de errores de guardado (opción C):
    0: Imagen guardada con éxito
    1: Cámara no inicializada
    2: No se ha capturado ninguna imagen
    3: La imagen aun no ha terminado de capturarse
    4: No se puede crear el archivo

  No existe un comando único para sacar una foto. La secuencia correcta sería:
    A: Solicitamos la captura de la imagen con los parámetros definidos
    C: Guardamos la imagen capturada en el sistema de archivos


  Recordemos la disposición de pines de comunicaciones de los distintos modelos:

                  ------SPI------- --I2C--
    Modelo        SCK MISO MOSI CS SCL SDA
    ------------  --- ---- ---- -- --- ---
    ESP8266       D5   D6   D7  D8 D1  D2
    ESP32         18   19   23   5 22  21    
  
  Resultado:
  Podemos cambiar la resolución de la imágen y sacar fotos.
*/

#include "Arduino.h"
#include "RoJoArduCAM.h"

//Definimos el pin CS de la cámara según el modelo de placa
#ifdef ESP32
  const byte pinCS=4;
#elif defined(ESP8266) //Si es ESP8266...
  const byte pinCS=D0;
#endif

//Creamos el objeto de gestión
RoJoArduCAM camera;

void help()
{
  //Muestra los comandos disponibles por el puerto serie

  Serial.println(F("Ayuda. Comandos disponibles."));
  Serial.println(F(" 0: Cambia la resolucion a 160x120"));
  Serial.println(F(" 1: Cambia la resolucion a 176x144"));
  Serial.println(F(" 2: Cambia la resolucion a 320x230"));
  Serial.println(F(" 3: Cambia la resolucion a 352x288"));
  Serial.println(F(" 4: Cambia la resolucion a 640x480"));
  Serial.println(F(" 5: Cambia la resolucion a 800x600"));
  Serial.println(F(" 6: Cambia la resolucion a 1024x768"));
  Serial.println(F(" 7: Cambia la resolucion a 1280x1024"));
  Serial.println(F(" 8: Cambia la resolucion a 1600x1200"));
  Serial.println(F(" A: Inicia la captura de una foto"));
  Serial.println(F(" B: Comprueba si ha finalizado la captura"));
  Serial.println(F(" C: Guarda la foto en sistema de archivos"));
  Serial.println(F(" D: Cambia a formato bmp"));
  Serial.println(F(" E: Cambia a formato jpg"));
  Serial.println(F(" H: Ayuda"));
}

void setup()
{
  Serial.begin(115200);

  //Inicializamos la cámara con el pin CS conectado al pin 4
  byte errorCode = camera.begin(pinCS);
  //Si tenemos algún error en la inicialización...
  if(errorCode)
  {
    Serial.print("Camara inicializada con error ");
    Serial.println(errorCode);
    //Led integrado configurado como salida
    pinMode(BUILTIN_LED,OUTPUT);
    //Led integrado apagado
    digitalWrite(BUILTIN_LED,HIGH);
    //Bucle infinito
    while(1)
    {
      //Repetimos tantas veces como código de error
      for(byte i=0;i<errorCode;i++)
      {
        digitalWrite(BUILTIN_LED,LOW); //Encendido
        delay(300);
        digitalWrite(BUILTIN_LED,HIGH); //Apagado
        delay(300);
      }
      delay(1000);
    };    
  }
  //Mostramos el listado de comandos disponibles
  help();
}

void loop()
{
  String filename;
  byte errorCode;

  //Si hay algún carácter en el buffer de entrada del puerto serie...
  if (Serial.available())
  {
    //Dependiendo del carácter...
    switch (Serial.read())
    {
      case '0':
        camera.setResolution(0);
        Serial.println(F("160x120"));
        break;
      case '1':
        camera.setResolution(1);
        Serial.println(F("176x144"));
        break;
      case '2':
        camera.setResolution(2);
        Serial.println(F("320x240"));
        break;
      case '3':
        camera.setResolution(3);
        Serial.println(F("352x288"));
        break;
      case '4':
        camera.setResolution(4);
        Serial.println(F("640x480"));
        break;
      case '5':
        camera.setResolution(5);
        Serial.println(F("800x600"));
        break;
      case '6':
        camera.setResolution(6);
        Serial.println(F("1024x768"));
        break;
      case '7':
        camera.setResolution(7);
        Serial.println(F("1280x1024"));
        break;
      case '8':
        camera.setResolution(8);
        Serial.println(F("1600x1200"));
        break;
      case 'A':
        camera.takePhoto();
        Serial.println(F("Captura iniciada"));       
        break;
      case 'B':
        if(camera.photoIsReady()) Serial.println(F("La captura ha terminado"));
        else Serial.println(F("La captura no ha terminado"));
        break;
      case 'C':
        //Tomamos como nombre de archivo el número de milisegundos transcurridos
        filename=String(millis());
        Serial.print(F("Guardando foto en archivo "));
        Serial.println(filename);
        //Resolución de la imagen guardada
        uint32_t resX,resY;
        //Solicitamos guardar la imagen en el archivo indicado
        //Se devuelve el código de error y su resolución
        //Los códigos de error son:
        // 0: No hubo errores
        // 1: Cámara no inicializada
        // 2: No se ha tomado ninguna foto
        // 3: La foto aun no ha terminado de capturarse
        // 4: No se puede crear el archivo
        errorCode=camera.savePhoto(filename,&resX,&resY);
        Serial.print(F("Foto guardada. Error = "));
        Serial.print(errorCode);
        Serial.print(F(". ancho="));
        Serial.print(resX);
        Serial.print(F(". alto="));
        Serial.println(resY);
        break;
      case 'D':
        camera.setFormatBMP();
        Serial.println(F("Cambiado a modo bmp"));
        break;
      case 'E':
        camera.setFormatJPG();
        Serial.println(F("Cambiado a modo jpg"));
        break;
      case 'H':
        help();
        break;        
      default:
        break;
    }
  }
}

