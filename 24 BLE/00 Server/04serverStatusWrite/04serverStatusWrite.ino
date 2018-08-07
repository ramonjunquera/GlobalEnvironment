/*
  Autor: Ramón Junquera
  Tema: BLE
  Fecha: 20180806
  Objetivo: Publicar un estado modificable en un servidor
  Material: placa ESP32 OLED TTGO 16Mb

  Descripción:
  Basado en el ejemplo anterior.
  Anadiremos a la característica la posibilidad de escribir el valor.
  Si sólo añadimos la propiedad de escritura, el cliente podrá modificar el valor, 
  pero no nos enteraremos de ello y no podremos ni cambiar el valor del estado ni
  aplicarlo al led.
  Para poder tenerlo en cuenta creamos una nueva clase heredada de las que utiliza
  una característiva en sus llamadas. Esta clase sólo tiene un destructor y dos
  metodos: onRead y onWrite. que son llamados justo después de que finalice la 
  acción de lectura o escritura del valor.
  A nosotros nos interesa escribir nuestro propio método onWrite para saber cuándo
  se escribe el valor.
  Cuando se le llama, se supone que el valor ya ha sido escrito.
  Para consultar el valor de una característica sólo tenemos la función getValue()
  que devuelve un std::string.
  Nosotros sólo utilizamos un byte para comunicar el estado y además sólo tienen
  significado los valores 0 y 1, así que tomaremos las siguientes reglas:
  - Sólo se tendrá en cuenta el primero de los caracteres del string (primer byte)
  - Si el valor del primer byte es mayor que 1, se considerará como 1.
  POr lo tanto, lo primero que hacemos en la rutina onWrite es consultar el valor
  actual leyendolo como si fuese un string.
  Después tomamos sólo su primer byte.
  Y si el primer byte es mayor que 1, reescribiremos el valor de la característica
  como 1.
  Después guardaremos el valor en la variable global del estado actual y aplicaremos
  el estado al led integrado.
  Por lo tanto, un cliente puede escribir el valor de la característica con cualquier
  tipo.
  
    
  Resultado:
  Se puede cambiar el estado del led escribiendo sobre el valor de la característica
*/

#include <Arduino.h>
#include "RoJoSwitch.h" //Gestión de interruptores
#include <BLEDevice.h> //Gestión de dispositivos
#include <BLEServer.h> //Gestión de servidores
#include <BLE2902.h> //Gestión de descriptores "Client Characteristic Configuration"

//Constantes globales
const byte LED_BUILTIN=2; //Led integrado del ESP32 OLED TTGO 16Mb
const byte pinSwitch=0; //Pin asignado a interruptor integrado en ESP32 OLED TTGO 16Mb

//Variables globales
byte currentStatus=0; //Comenzamos con el led apagado. 0=apagado, 1=encendido
RoJoSwitch mySwitch(pinSwitch); //Objeto de gestión del interruptor
BLECharacteristic *pCharacteristic; //Caracteristica BLE. Necesaria para asignar su valor

//Creamos una clase heredada de la que gestiona las llamadas de una característica
//La adaptaremos para gestionar las peticiones de escritura
class MyCharacteristicCallbacks: public BLECharacteristicCallbacks
{
  //Reescribimos el método que escribe el valor
  //Este método es llamado justo después de recibir un nuevo valor y guardarlo en
  //la característica
  void onWrite(BLECharacteristic *pCharacteristic)
  {
    //Anotamos el valor recibido
    //Sólo existe un método para ello y devuelve un std::string
    std::string rxString=pCharacteristic->getValue();
    //Puesto que nosotros enviuamos un byte, sólo nos interesa el primero del los
    //caracteres del String
    byte rxValue=rxString[0];
    //El valor recibido debe ser 0 o 1
    //Si el valor recibido es mayor que 1...
    if(rxValue>1)
    {
      //...lo tomaremos como 1
      rxValue=1;
      //Y lo guardamos como valor actual
      pCharacteristic->setValue(&rxValue,1);
    }
    //Ahora el valor recibido es 0 o 1

    //El estado actual será igual que el valor recibido
    currentStatus=rxValue;
    //Aplicaremos el estado recibido al led integrado
    digitalWrite(LED_BUILTIN,currentStatus);
  }
};

void setup()
{
  Serial.begin(115200);
  //Asignamos un nombre descriptivo al dispositivo
  //Para guardar compatibilidad intentaremos que siempre tenga un máximo de 5 caracteres
  BLEDevice::init("ESP32");
  //Creamos un servidor dentro del dispositivo
  BLEServer *pServer = BLEDevice::createServer();
  //Creamos un servicio en el servidor al que asignamos un identificador
  BLEService *pService = pServer->createService("4FAFC201-1FB5-459E-8FCC-C5C9C331914B");
  //Creamos una característica para el servicio a la que le asignamos un identificador
  //También indicamos qué tipo de acceso tendrá (propiedad). En este caso se podrá consultar
  //a petición (READ), podrá enviar notificaciones (NOTIFY) y se podrá modificar el
  //valor (WRITE)
  pCharacteristic = pService->createCharacteristic("BEB5483E-36E1-4688-B7F5-EA07361B26A8",BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_NOTIFY);
  //Aplicamos a la característica nuestro propio sistema de gestión de llamadas definido
  //anteriormente que se encargue de las escrituras del valor (modificaciones)
  pCharacteristic->setCallbacks(new MyCharacteristicCallbacks());
  //Asignamos el valor del estado actual a la característica
  //No hay una función para asignar un byte, pero sí un array de bytes
  //Tomamos el puntero de la variable y sólo escribimos 1 byte
  pCharacteristic->setValue(&currentStatus,1);
  //Añadimos un descriptor a la característica del tipo "Client Characteristic Configuration"
  //Este descriptor nos da la posibilidad de que el cliente pueda recibir notificaciones en tiempo real
  pCharacteristic->addDescriptor(new BLE2902());
  //Iniciamos el servicio
  pService->start();
  //El servidor podrá ser publicado y visible
  BLEAdvertising *pAdvertising = pServer->getAdvertising();
  //Añadimos el número de identificación del servicio para que sea publicado y visible
  pAdvertising->addServiceUUID(pService->getUUID());
  //Iniciamos la publicación del servidor
  pAdvertising->start();
  //Informamos que ya está disponible el servicio BLE
  Serial.println("\nBLE iniciado");

  //Configuramos el led como salida
  pinMode(LED_BUILTIN,OUTPUT);
  //Asignamos el estado inicial
  digitalWrite(LED_BUILTIN,currentStatus);
  
}

void loop()
{
  //Si se ha pulsado el botón...
  if(mySwitch.pressed())
  {
    //...cambiamos el estado actual
    currentStatus=1-currentStatus;
    //Aplicamos el nuevo estado el led integrado
    digitalWrite(LED_BUILTIN,currentStatus);
    //Cambiamos el valor de la característica BLE
    pCharacteristic->setValue(&currentStatus,1);
    //Enviamos una notificación a cualquier cliente que esté conectado
    pCharacteristic->notify();
  }
}
