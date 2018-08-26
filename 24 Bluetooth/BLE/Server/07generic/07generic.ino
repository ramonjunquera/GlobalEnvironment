/*
  Autor: Ramón Junquera
  Tema: BLE
  Fecha: 20180818
  Objetivo: Creación de un servicio/característica geneŕico
  Material: placa ESP32

  Descripción:
  Crearemos un servidor con el servicio genérico del tipo org.bluetooth.service.battery_service
  https://www.bluetooth.com/specifications/gatt/viewer?attributeXmlFile=org.bluetooth.service.battery_service.xml
  Este servicio informa del nivel de la batería del dispositivo.
  El identificador del servicio es fijo por ser genérico.
  El identificador corto es 0x180F que corresponde con el largo 0000180F-0000-1000-8000-00805f9b34fb
  
  La regla general para pasar un identificador corto a uno largo es sustituir las X de
  0000XXXX-0000-1000-8000-00805f9b34fb por los 4 dígitos hexadecimales del código corto (0xXXXX).

  Según se indica en el estándar, este servicio tiene una sóla característica también genérica
  del tipo org.bluetooth.characteristic.battery_level
  https://www.bluetooth.com/specifications/gatt/viewer?attributeXmlFile=org.bluetooth.characteristic.battery_level.xml
  Su identificador corto es 0x2A19 y el largo 00002A19-0000-1000-8000-00805f9b34fb
  El valor de esta característica es un sólo byte que representa el porcentaje de nivel de
  batería. 0=0%=vacía, 100=100%=llena
  El valor es sólo de lectura.
  La propiedad de notificación es opcional. En este ejemplo no la incluiremos para simplificar.
  
  Tampoco incluiremos un identificador en la publicación (no es obligatorio).

  Resultado:
  El servidor incluye un servicio genérico para consultar el nivel de batería
*/

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
  //El servicio tiene un código predeterminado
  //Corresponde con un servicio genérico del tipo org.bluetooth.service.battery_service
  //https://www.bluetooth.com/specifications/gatt/viewer?attributeXmlFile=org.bluetooth.service.battery_service.xml
  BLEService *pService = pServer->createService("0000180F-0000-1000-8000-00805f9b34fb");
  //Este servicio tiene una sóla característica de lectura, también genérica
  //del tipo org.bluetooth.characteristic.battery_level
  //https://www.bluetooth.com/specifications/gatt/viewer?attributeXmlFile=org.bluetooth.characteristic.battery_level.xml
  //La propiedad de notificación es opcional. Para simplificar, no lo incluiremos.
  BLECharacteristic *pCharacteristic = pService->createCharacteristic("00002A19-0000-1000-8000-00805f9b34fb",BLECharacteristic::PROPERTY_READ);
  //El valor es de tipo byte y contiene el porcentaje de batería
  //Para este ejemplo fijaremos un valor
  byte battery=23;
  //No existe una función para escribir como valor un byte. Recurrimos a escribir un determinado número de 
  //bytes desde una posición de memoria
  pCharacteristic->setValue(&battery,1);
  //Iniciamos el servicio
  pService->start();
  //El servidor podrá ser publicado y visible
  BLEAdvertising *pAdvertising = pServer->getAdvertising();
  //En esta ocasión no incluiremos un identificador para la publicación
  //Iniciamos la publicación del servidor
  pAdvertising->start();
  //Informamos que ya está disponible el servicio BLE
  Serial.println("\nBLE iniciado");
}

void loop()
{
  //Nada especial que hacer aquí
}
