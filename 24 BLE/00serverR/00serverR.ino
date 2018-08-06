/*
  Autor: Ramón Junquera
  Tema: BLE
  Fecha: 20180805
  Objetivo: Creación de un servidor simple
  Material: placa ESP32

  Descripción:
  En este ejercicio crearemos un servidor simple con un servicio customizado, con una sola característica,
  que entrega un valor constante (de sólo lectura) al cliente.

  Los identificadores (UUID), tanto del servicio como de su característica pueden tener cualquier valor puesto
  que son customizados.

  Los pasos a seguir son:
  - Inicialización del dispositivo
  - Creación del servidor
  - Creación del servicio
  - Creación de la característica
  - Inicio del servicio
  - Publicación del servidor

  La publicación del servidor es imprescindible. Si no lo hacemos, no podrá ser detectado.

  Resultado:
  Se puede consultar el valor de la característica, del servicio, del servidor BLE.
*/

#include <Arduino.h>
#include <BLEDevice.h> //Gestión de dispositivos
#include <BLEServer.h> //Gestión de servidores

void setup()
{
  Serial.begin(115200);
  //Asignamos un nombre descriptivo al dispositivo
  BLEDevice::init("MyESP32");
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
  //Iniciamos la publicación del servidor
  pAdvertising->start();
  //Informamos que ya está disponible el servicio BLE
  Serial.println("\nBLE iniciado");
}

void loop()
{
  //Nada especial que hacer aquí
}
