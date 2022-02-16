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
  //También indicamos qué tipo de acceso tendrá (propiedad). En este caso sólo recibirá
  //el valor por notificaciones
  pCharacteristic = pService->createCharacteristic("BEB5483E-36E1-4688-B7F5-EA07361B26A8",BLECharacteristic::PROPERTY_NOTIFY);
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