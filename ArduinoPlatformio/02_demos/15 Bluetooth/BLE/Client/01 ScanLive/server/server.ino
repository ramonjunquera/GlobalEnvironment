#include <Arduino.h>
#include <BLEDevice.h> //Gestión de dispositivos
#include <BLEServer.h> //Gestión de servidores

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
  //También indicamos qué tipo de acceso tendrá (propiedad). En este caso es de sólo lectura
  BLECharacteristic *pCharacteristic = pService->createCharacteristic("BEB5483E-36E1-4688-B7F5-EA07361B26A8",BLECharacteristic::PROPERTY_READ);
  //Asignamos un valor a la característica: un texto
  pCharacteristic->setValue("Hola");
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
}

void loop()
{
  //Nada especial que hacer aquí
}
