/*
  Autor: Ramón Junquera
  Tema: Bluetooth
  Fecha: 20180822
  Objetivo: Recepción contínua en un servidor (sensor)
  Material: ESP32 OLED TTGO 16Mb

  Descripción:
  Para este ejemplo utilizaremos la aplicación SensoDuino instalada en el móvil como cliente.
  Esta applicación permite utilizar todos los sensores del móvil y transmitir sus medidas
  a través de bluetooth.

  En SensoDuino se pueden activar por separado cualquiera de los sensores (primer check).
  También se pueden enviar esos datos por bluetooth (segundo check).
  Y se pueden guardar los valores recogidos en un archivo de texto (tercer check).

  Nosotros sólo probaremos a activar y enviar alguno de los sensores.
  
  En el servidor recogeremos los valores y haremos una representación gráfica.
  Para ello utilizaremos un ESP32 con una pantalla OLED monocroma integrada (ESP32 OLED TTGO 16Mb).
  
  Listado de tipos de sensores:
   1- ACCELEROMETER  (m/s^2 - X,Y,Z)
   2- MAGNETIC_FIELD (uT - X,Y,Z)
   3- ORIENTATION (yaw, pitch, roll)
   4- GYROSCOPE (rad/s - X,Y,Z)
   5- LIGHT (SI lux)
   6- PRESSURE (hPa millibar)
   7- DEVICE TEMPERATURE (C)
   8- PROXIMITY (Centimeters or 1,0)
   9- GRAVITY (m/s^2 - X,Y,Z)
  10- LINEAR_ACCELERATION (m/s^2 - X,Y,Z)
  11- ROTATION_VECTOR (X,Y,Z)
  12- RELATIVE_HUMIDITY (%)
  13- AMBIENT_TEMPERATURE (C)
  14- MAGNETIC_FIELD_UNCALIBRATED (uT - X,Y,Z)
  15- GAME_ROTATION_VECTOR (X,Y,Z)
  16- GYROSCOPE_UNCALIBRATED (rad/s - X,Y,Z)
  17- SIGNIFICANT_MOTION (1,0)
  97 - AUDIO (Vol.)
  98 - GPS1 (lat., long., alt.)
  99 - GPS2 (bearing, speed, date/time)

  A continuación mostramos un ejemplo de la información que se recibe de Sensoduino:
  >3,123,353.50443,-12.071683,-0.491955
  
  > significa que se inicia un comando válido
  3 es el tipo de sensor
  , separador de parámetros
  123 contador
  , separador de parámetros
  353.50443 valor decimal del primer parámetro
  , separador de parámetros
  -12.071683 valor decimal del segundo parámetro
  , separador de parámetros
  -0.491955 valor decimal del tercer parámetro
  <10> Salto de linea. Carácter 10. Siempre para finalizar una cadena de datos

  Los valores 99.99 no son válidos y deberían ser ignorados

  Nosotros nos interesaremos concretamente en el sensor 3 (Orientation), que indica la posición
  en la que se encuentra el móvil.
  Su sintaxis sería:
  >sensor,contador,Yaw,Pitch,Roll
  Los tres ángulos se miden en grados.

  Si tomamos el móvil en apaisado, con el borde izquierdo en la parte inferior y en vertical, obtenemos estos valores ideales:
  >3,XXX,???,0,90
  En esta posición Yaw indicaría el giro relativo al norte. No es un valor que nos interese.
  Pitch = 0. Indica la inclinación a derecha o a izquierda. Un giro a la izquierda hará el valor positivo 
    y a la derecha, negativo.
  Roll = 90. Indica la inclinación hacia atrás o hacia adelante. En vertical es 90. En horizontal 0.
    Más inclinado de horizontal comenzará a ser negativo

  ESP32 es suficientemente rápido como para recibir, parsear, calcular y dibujar recibiendo actualizaciones
  cada décima de segundo (100 milisegundos).

  Nota. SensoDuino es una excelente herramienta, pero consume muchísima batería, No es conveniente utilizarlo durante mucho
  tiempo si no se está recargando el móvil.

  Resultado:
  Una bola dibujada en el display indica la inclinación del móvil respecto de la horizontal en tiempo real
*/

#include <Arduino.h>
#include "BluetoothSerial.h" //Gestión de Bluetooth
#include "RoJoSSD1306.h" //Librería de gestión del display SSD1306
#include "RoJoSprite.h" //Librería de gestión de sprites monocromos

//Constantes globales

//Variables globales
BluetoothSerial BT; //Objeto de gestión del bluetooth
RoJoSSD1306 display; //Objeto de gestión del display
RoJoSprite mySprite; //Objeto de gestión del sprite
String rxString=""; //Cadena de texto recibido

void setup()
{
  Serial.begin(115200);
  //Activamos el Bluetooth
  BT.begin("ESP32");
  //Inicializamos el display con los pines en los que está conectado la placa
  //En este modelo no son seleccionables y no coinciden con los estandar:
  //void begin(byte pinCS_SD,byte pinSDA,byte pinSCL,byte pinRST);
  //Nota es este ejemplo no se utiliza una tarjeta SD para almacenar los archivos
  //por lo tanto el pinCS_SD no se tiene en cuenta. Podríamos escribir cualquier valor
  display.begin(SS,4,15,16);

  //Dimensionamos el sprite. Anchura=4. Páginas=1
  mySprite.setSize(4,1);
  //Lo definimos
  //void setPage(int16_t x,int16_t page,byte mask,byte color);
  mySprite.drawPage(0,0,0b00000110,4); //4=escribir el valor tal cual
  mySprite.drawPage(1,0,0b00001111,4);
  mySprite.drawPage(2,0,0b00001111,4);
  mySprite.drawPage(3,0,0b00000110,4);
}

int8_t sign(float a)
{
  //Función que indica el signo de un valor float
  return (a>0)-(a<0); 
}

void loop()
{
  //Si hay algún dato recibido pendiente de ser procesado...
  if (BT.available())
  {
    //...lo leemos
    byte rxByte=BT.read();
    //Si el carácter recibido no es el 10 (fin de línea)...añadimos el carácter a la cadena
    if(rxByte!=10) rxString+=(char)rxByte;
    //Si hemos recibido el fin de línea...
    else
    {
      //Declaramos a este nivel la variable en las que guardaremoe el pitch y el roll
      float pitch,roll;
      //Debemos parsear el texto recibido
      //Si comienza por un >
      if(rxString[0]=='>')
      {
        //Comienza por el carácter esperado. Lo eliminamos
        rxString=rxString.substring(1);
        //Intentamos localizar la primera coma
        int16_t posComma=rxString.indexOf(',');
        //Si hemos encontrado la coma...
        if(posComma>=0)
        {
          //Obtenemos el tipo de sensor que ha generado el dato
          byte sensorType=rxString.toInt();
          //Si el sensor es el 3 (Orientation)...
          if(sensorType==3)
          {
            //...es el sensor esperado
            //Eliminamos el prefijo con el tipo de sensor y su coma
            rxString=rxString.substring(posComma+1);
            //El siguiente dato es un contador que no tiene importancia para nosotros
            //Intentamos localizar la siguiente coma
            posComma=rxString.indexOf(',');
            //Si hemos encontrado la coma...
            if(posComma>=0)
            {
              //Directamente eliminados el contador y su coma
              rxString=rxString.substring(posComma+1);
              //El siguiente dato es el Yaw, que tampoco lo tendremos en cuenta
              //Intentamos localizar la siguiente coma
              posComma=rxString.indexOf(',');
              //Si hemos encontrado la coma...
              if(posComma>=0)
              {
                //Directamente eliminados el yaw y su coma
                rxString=rxString.substring(posComma+1);
                //El siguiente dato es el pitch, que sí nos interesa
                //Intentamos localizar la siguiente coma
                posComma=rxString.indexOf(',');
                //Si hemos encontrado la coma...
                if(posComma>=0)
                {
                  //Obtenemos el pitch
                  pitch=rxString.toFloat();
                  //Eliminamos el prefijo del pitch y su coma
                  rxString=rxString.substring(posComma+1);
                  //Sólo nos debería quedar el roll. Lo obtenemos
                  roll=rxString.toFloat();
                  //Tenemos todos los datos que nos interesan
   
                  //Deberíamos fijar ciertos límites en los datos recibidos para no dibujar
                  //fuera de pantalla. Supondremos que nunca deberíamos recibir un valor
                  //superior a 20 grados (en valor absoluto).
                  if(abs(pitch)>20) pitch=20*sign(pitch);
                  if(abs(roll)>20) roll=20*sign(roll);

                  //Recordemos que la resolución del display es 128x64

                  //Cálculo de las coordenadas horizontales (roll)
                  //El roll es la inclinación a derecha o izquierda
                  //El roll a la izquierda es positivo. A la derecha, negativo.
                  //Puesto que el sprite tiene un tamaño de 4x4 el rango de coordenadas
                  //debe ser: [0,124]
                  //Se debe convertir el rango [20,-20] a [0,124]
                  //int x=(-roll+20)*124/40;
                  int x=(-roll+20)*3.1;

                  //Cálculo de las coordenadas verticales (pitch)
                  //El pitch es la inclinación hacia arriba o abajo
                  //El pitch hacia arriba es positivo. Hacia abajo negativo.                   
                  //Puesto que el sprite tiene un tamaño de 4x4 el rango de coordenadas
                  //debe ser: [0,60]
                  //Se debe convertir el rango [20,-20] a [0,60]
                  //int y=(-pitch+20)*60/40;
                  int y=(-pitch+20)*1.5;

                  //Dibujamos el sprite
                  display.videoMem->clear();
                  display.videoMem->drawSprite(x,y,&mySprite,1);
                  display.show();                  
                }
              }
            }
          }
        }
      }
      //Ya hemos procesado este string. Lo reseteamos
      rxString="";
    }
  }
}
