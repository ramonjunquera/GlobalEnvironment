/*
  Autor: Ramón Junquera
  Tema: Bluetooth
  Fecha: 20180827
  Objetivo: Modificar la configuración del módulo 
  Material: Placa Wemos D1 R2, módulo HC-05
  
  Descripción:
  El circuito está preparado para que el dispositivo HC-05 arranque en modo comandos.
  Para ello el pin KEY está conectado directamente a 3.3V.
  El pin VCC de alimentación está conectado a 5V.
  Según las especificaciones del fabricante, trabaja con un rango de alimentación de 3.6 a 6V.
  Por lo tanto 3.3V no es suficiente.
  El resto de pines que son de señal, funcionan perfectamente con 3.3V.

  Puesto que el módulo está conectado a alimentación permanentemente, el reseteo debe ser
  manual: desconectarlo y volverlo a conectar.

  El programa conecta directamente el módulo HC-05 con el puerto serie.
  De esta manera, se pueden escribir comandos directamente desde la línea del
  puerto seriel al módulo bluetooth. También se reciben las respuestas.

  Para conectar con el módulo HC-05 creamos un puerto serie por software. Puesto que los puertos
  serie por software tienen limitaciones de velocidad y la velocidad por defecto del puerto 
  serie de HC-05 es de 38400 baudios, usaremos esta velocidad.

  Sabremos que funciona porque al enviar el comando AT recibimos como respuesta OK

  Si no funciona deberíamos comprobar lo siguiente:
  - Que la velocidad de la ventana del terminal serie sea la correcta (38400)
  - Que la ventana del terminal tenga los caracteres NL & CR como indicadores de fin de línea
  Si con texto no se consigue respuesta es posible que sea porque el adaptador está configurado a una velocidad distinta
  de la indicada.
  Por defecto el adaptador viene configurado a 38400 baudios. Si no fuese tu caso, prueba con otras velocidades.
  
  Es importante tener en cuenta que el programa configura tanto el puerto serie por software como por hardware a la 
  misma velocidad.
  Si configurásemos el puerto serie hardware a la máxima velocidad, no tendríamos problemas de comunicación con la placa
  pero nuestros comandos llegarían mucho más rápidos de lo que se pueden enviar al adaptador.
  Esto en secuencias cortas, como "AT" no tiene importancia, pero con comandos más largos produce errores que se traducen
  en que el adaptador no "entiende" los comandos que le enviamos y siempre responde con un ERROR(0) = "comando AT no válido".
  
  Para comprobar que la configuración que hemos aplicado funciona correctamente, podemos utilizar el mismo
  programa, pero debemos desconectar el pin KEY y reiniciar para que arranque en modo datos.
  Por otra parte podemos utilizar un móvil como cliente con la aplicación "Serial Bluetooth Terminal".

  Resultado:
  Podemos interactuar directamente con el adaptador

  Ejemplo:
  Cómo cambiar el nombre del dispositivo.
  Comprobamos que tenemos establecida una conexión de comandos.
  Comando: AT
  Respuesta: OK
  Reseteamos el adaptador a los valores por defecto
  Comando: AT+ORGL
  Respuesta: OK
  Solicitamos el nombre actual del dispositivo
  Comando: AT+NAME
  Respuesta: +NAME:H-C-2010-06-01
             OK
  Asignamos el nombre que queremos que tenga
  Comando: AT+NAME=HC05
  Respuesta: OK
  Solicitamos de nuevo el nombre para comprobar que es correcto
  Comando: AT+NAME
  Respuesta: +NAME:HC05
             OK

  Nota:
  El dispositivo HC-05 tiene como contraseña por defecto 1234
*/

#include <Arduino.h>
#include <SoftwareSerial.h>

//Definición de constantes globales
const byte pinTX = D6;
const byte pinRX = D7;
const long serialSpeed = 38400;

//Definición de objeto para el puerto serie software que nos comunicará con el adaptador
//Los pines RX y TX se intercambian
SoftwareSerial BT(pinTX,pinRX);

void setup()
{
  //Activamos el puerto serie hardware
  Serial.begin(serialSpeed);
  //Activamos el puerto serie software
  BT.begin(serialSpeed);
  //Indicamos que estamos preparados
  Serial.println("\nReady!");
}

void loop()
{
  //Si hay datos pendientes de recibir por el puerto serie...recogemos un byte y lo enviamos por el bluetooth
  if (Serial.available()) BT.write(Serial.read());
  //Si hay datos pendientes de recibir por el bluetooth...recogemos un byte y lo enviamos por el puerto serie
  if (BT.available()) Serial.write(BT.read());
}

