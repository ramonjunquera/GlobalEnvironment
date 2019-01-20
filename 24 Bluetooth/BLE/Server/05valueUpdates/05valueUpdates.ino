/*
  Autor: Ramón Junquera
  Tema: BLE
  Fecha: 20180807
  Objetivo: Actualizar periódicamente un valor
  Material: placa ESP32 OLED TTGO 16Mb

  Descripción:
  Crearemos un servidor, con un servicio y una característica cuyo valor representa
  el tiempo en milisegundos transcurrido desde el último reinicio.
  La característica tendrá un descriptor que permita que el cliente reciba 
  notificaciones.
  El valor se actualizará cada segundo y el cliente recibirá el valor en cada
  actualización.

  Puesto que el valor enviado se muestra en milisegundos, es normal que no varíen
  las 3 cifras más bajas.

  La única complicación que tiene el desarrollo es que para facilitar la lectura
  del valor, se guarda como string y no como uint32_t.
  Los valores no permiten guardar el tipo String (System::String).
  Sólo permite std:string.
  String y std:string no son compatibles entre sí y no existe una función conversora.
  Por eso la tenemos que crear nosotros mismos.
    
  Resultado:
  El valor se actualiza cada segudo gracias a las notificaciones
*/

#include <Arduino.h>
#include <BLEDevice.h> //Gestión de dispositivos
#include <BLEServer.h> //Gestión de servidores
#include <BLE2902.h> //Gestión de descriptores "Client Characteristic Configuration"

//Variables globales
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
  //También indicamos qué tipo de acceso tendrá (propiedad). En este caso se podrá
  //solicitar el valor y se actualizará por notificaciones
  pCharacteristic = pService->createCharacteristic("BEB5483E-36E1-4688-B7F5-EA07361B26A8",BLECharacteristic::PROPERTY_NOTIFY | BLECharacteristic::PROPERTY_READ);
  //Asignamos el valor inicial
  valueUpdate();
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
}

void valueUpdate()
{
  //Actualiza el valor de la característica

  //Anotamos el tiempo transcurrido en milisegundos
  uint32_t currentValue=millis();
  //El valor de una característica permite guardar casi cualquier tipo
  //Podríamos guardarlo con el tipo uint32_t, pero cuando lo vemos en la
  //aplicación del móvil como cliente lo visualizamos en hexadecimal.
  //Este sistema no es muy práctico para ser leido por humanos.
  //La función setValue no permite guardar como String (System::String), sólo
  //como std::string
  //Convertiremos el valor numérico a std::string y lo guardaremos como valor.
  //Primero lo convertimos a String y después con c_str a string.
  pCharacteristic->setValue(String(currentValue).c_str());
  //Lo enviamos por el puerto serie para debug
  Serial.println(currentValue);
}

void loop()
{
  //Actualizamos el valor
  valueUpdate();
  //Enviamos una notificación a cualquier cliente que esté conectado
  pCharacteristic->notify();
  //Esperamos un momento
  delay(1000);
}
