/*
  Autor: Ramón Junquera
  Fecha: 20210717
  Tema: Comunicaciones I2C
  Objetivo: Aprender cómo funcionan el protocolo I2C

  Descripción:
  El protocolo de comunicaciones I2C sólamente consta de 2 cables. Uno para la señal del reloj (SCL)
  y otro para la señal de datos (SDA).
  Todas las placas están preparadas para trabajar con este protocolo, pero los pines de
  conexión dependen de cada modelo.

  En el IDE de Arduino la librería Wire.h es la que gestiona el protocolo I2C.

  Si hay más de un dispositivo I2C, se pueden conectar todos en paralelo a estos mismos cables.
  Para saber a qué dispositivo le enviamos los datos, cada uno de ellos tienen un identificador distinto.
  El identificador no es dinámico. Cada dispositivo I2C tiene asociado su propio identificador.
  Si se conectan varios dispositivos iguales, no se podrá enviar información sólo a uno de ellos.
  Se contempla que se pueden conectar hasta un máximo de 127 dispositivos distintos.
  No se sabe de antemano el identificador de cada dispositivo.
  Para averiguarlo hemos desarrollado el siguiente programa.
  Mostrará por el monitor serie el identificador de los dispositivos encontrados.

  Resultado:
  Se muestra por el monitor serie los identificadores de los dispositivos encontrados.

  Notas:
  - El sensor VL53L1V2 utilizado para le prueba tiene el identificador 0x29
  - En el ejemplo con el M5Stack Watch sólo se detectará el sensor VL53L1V2
  - En el ejemplo con el ESP32OLED se detectará además el display OLED con el identificador 0x3C.
    En este caso es obligatorio resetear el display para su activación
*/

#include <Arduino.h>
#include <Wire.h> //Gestión de I2C

void setup() {
  //Activamos el puerto serie
  Serial.begin(115200);
}

void loop() {
  int nDevices;

  //Indicamos que comenzamos a escanear el bus I2C
  Serial.println(F("Scanning..."));

  //Inicializamos la librería de comunicaciones despendiendo del dispositivo
  Serial.print("Dispositivo: ");
  #ifdef ARDUINO_M5Stick_Watch
    Wire.begin(13,25); //Configuración para M5Stack Watch
    Serial.println("ARDUINO_M5Stick_Watch");
  #elif defined(ESP32OLED16MB)
    //Necesario resetear el display para activarlo
    pinMode(16,OUTPUT);
    digitalWrite(16,LOW);
    delay(50);
    digitalWrite(16,HIGH);
    Wire.begin(4,15); //Configuración para ESP32OLED
    Serial.println("ESP32OLED16MB");
  #else //Para el resto de dispositivos (UNO)
    Serial.println("otros");
    Wire.begin(); //Configuración con pines estándar
  #endif

  //Inicialmente no hemos encontrado ningún dispositivo
  nDevices = 0;
  //Recorremos todos los posibles identificadores de dispositivos...
  for(byte address=1;address<127;address++) {
    //Activamos la conexión con el identificador procesado
    Wire.beginTransmission(address);
    //Sin enviar ningún dato, cerramos las comunicaciones tomando nota del posible error
    byte error = Wire.endTransmission();
    //Si no se ha producido ningún error...
    if (!error) {
      //...es porque el dispositivo ha respodido correctamente!
      //Informamos del descubrimiento
      Serial.print(F("I2C device found at address 0x"));
      //Si el número sólo contiene un dígito hexadecimal, le añadimos nosotros un cero al principio
      if (address<16) Serial.print(F("0"));
      //Enviamos el identificador por el puerto serie
      Serial.println(address,HEX);
      
      //Hemos encontrado un nuevo dispositivo conectado
      nDevices++;
    }
    else if (error==4) { //Si se ha producido un error y es el 4...
      //No sabemos qué ha podido ocurrir. Informamos.
      Serial.print("Unknow error at address 0x");
      //Añadimos el cero al principio por tener un sólo dígito
      if (address<16) Serial.print("0");
      Serial.println(address,HEX);
    }
  }
  //Hemos terminado de recorrer todas las posibles direcciones del bus I2C

  //Si no hemos encontrado dispositivos...informamos
  if (nDevices == 0) Serial.println("No I2C devices found\n");
  //Hemos encotrado algún dispositivo...informamos que hemos terminado
  else Serial.println("done\n");

  //Esperamos 5 segundos...
  delay(5000);
  //...y volvemos a repetir el scan
}