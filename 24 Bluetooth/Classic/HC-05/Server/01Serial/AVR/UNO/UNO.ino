/*
  Autor: Ramón Junquera
  Tema: Bluetooth
  Fecha: 20180827
  Objetivo: Modificar la configuración del módulo 
  Material: Placa Arduino UNO, módulo HC-05
  
  Descripción:
  El programa permite arrancar el módulo HC-05 en modo comandos y lo conecta directamente con el
  puerto serie. De esta manera, se pueden escribir comandos directamente desde la línea del
  puerto seriel al módulo bluetooth. También se reciben las respuestas.

  Para no depender de nada, se controla la alimentación (reinicio) del módulo desde un pin.

  Por defecto, cuado HC-05 arranca lo hace en modo datos. Nos comunicamos con él a través de 
  un puerto serie. Esto ocurre cuando el pin KEY no está conectado o está conectado a LOW.
  Podemos saber que está en modo datos porque el led integrado parpadea rápidamente (varias
  veces por segundo).

  Para forzar el arranque en modo de comandos el pin KEY debe estar en HIGH.
  Reconocemos el modo porque el led integrado parpadea lento (una vez cada dos segundos).

  El programa se encarga quitar la alimentación del módulo, activar el pin KEY, alimentarlo de
  nuevo para que reinicie.

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
  programa, pero debemos desconectar el pin KEYy reiniciar para que arranque en modo datos.
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
const byte pinTX = 11;
const byte pinRX = 12;
const byte pinKEY = 8;
const byte pinVCC = 9;
const long serialSpeed = 38400;

//Definición de objeto para el puerto serie software que nos comunicará con el adaptador
//Los pines RX y TX se intercambian
SoftwareSerial BT(pinTX,pinRX);

void setup()
{
  //Activamos el puerto serie hardware
  Serial.begin(serialSpeed);
  //Definimos el pin KEY como salida
  pinMode(pinKEY,OUTPUT);
  //Definimos el pin VCC como salida
  pinMode(pinVCC,OUTPUT);
  //Nos aseguramos que el modulo no tenga alimentación
  digitalWrite(pinVCC,LOW);
  //Activamos el pin KEY para que entre en modo comandos
  digitalWrite(pinKEY,HIGH);
  //Activamos la alimentación del adaptador
  digitalWrite(pinVCC,HIGH);
  //Activamos el puerto serie software
  BT.begin(serialSpeed);
  //Indicamos que estamos preparados
  Serial.println("Ready!");
}

void loop()
{
  //Si hay datos pendientes de recibir por el puerto serie...recogemos un byte y lo enviamos por el bluetooth
  if (Serial.available()) BT.write(Serial.read());
  //Si hay datos pendientes de recibir por el bluetooth...recogemos un byte y lo enviamos por el puerto serie
  if (BT.available()) Serial.write(BT.read());
}

