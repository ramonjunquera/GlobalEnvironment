/*
  Autor: Ramón Junquera
  Tema: BLE
  Fecha: 20180806
  Objetivo: Actualizar periódicamente un valor
  Material: placa ESP32 OLED TTGO 16Mb

  Descripción:
  Basado en el ejemplo anterior.
  Enviar un valor periódicamente consume energía.
  En este ejercicio sólo actualizaremos el valor y lo notificaremos si hay
  algún cliente conectado.

  Para controlar si tenemos clientes conectados creamos una clase heredada de las
  llamadas de un servidor.
  La clase original sólo tiene 3 métodos: un destructor, onConnect y onDisconnect.
  Los métodos onConnect y onDisconnect se llaman justo después de ejecutar la 
  acción de nueva conexión de cliente o desconexión de cliente.
  Reescribimos estos métodos para que gestionen el valor de la variable globar que
  mantiene el número de conexiones actuales.

  En la función loop sólo tendremos en cuenta las actualizaciones si tenemos 
  conectado algún cliente.
    
  Resultado:
  Mirando el monitor del puerto serie podemos comprobar que sólo se envían notificaciones
  si hay algún cliente conectado.
*/

#include <Arduino.h>
#include <BLEDevice.h> //Gestión de dispositivos
#include <BLEServer.h> //Gestión de servidores
#include <BLE2902.h> //Gestión de descriptores "Client Characteristic Configuration"

//Variables globales
BLECharacteristic *pCharacteristic; //Caracteristica BLE. Necesaria para asignar su valor
byte clientsCount=0; //Número de clientes conectados

class MyServerCallbacks: public BLEServerCallbacks
{
  //Creamos una clase heredada de la clase que gestiona las llamadas del servidor
  
  void onConnect(BLEServer* pServer)
  {
    //Método llamado después de que se ha conectado un cliente

    //Aumentamos el número de clientes conectados
    clientsCount++;
  };
  void onDisconnect(BLEServer* pServer)
  {
    //Método llamado después de que se ha desconectado un cliente

    //Disminuimos el número de clientes conectados
    clientsCount--;
  }
};

void setup()
{
  Serial.begin(115200);
  //Asignamos un nombre descriptivo al dispositivo
  BLEDevice::init("MyESP32");
  //Creamos un servidor dentro del dispositivo
  BLEServer *pServer = BLEDevice::createServer();
  //Modificamos la gestión de las llamadas del servidor con nuestra propia clase
  pServer->setCallbacks(new MyServerCallbacks());
  //Creamos un servicio en el servidor al que asignamos un identificador
  BLEService *pService = pServer->createService("4FAFC201-1FB5-459E-8FCC-C5C9C331914B");
  //Creamos una característica para el servicio a la que le asignamos un identificador
  //También indicamos qué tipo de acceso tendrá (propiedad). En este caso se podrá
  //actualizar el valor por notificaciones
  pCharacteristic = pService->createCharacteristic("BEB5483E-36E1-4688-B7F5-EA07361B26A8",BLECharacteristic::PROPERTY_NOTIFY);
  //Añadimos un descriptor a la característica del tipo "Client Characteristic Configuration"
  //Este descriptor nos da la posibilidad de que el cliente pueda recibir notificaciones en tiempo real
  pCharacteristic->addDescriptor(new BLE2902());
  //Iniciamos el servicio
  pService->start();
  //El servidor podrá ser publicado y visible
  BLEAdvertising *pAdvertising = pServer->getAdvertising();
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
  //Convertiremos el valor numérico en std::string y lo guardaremos como valor.

  //Convertimos el valor numérico en String
  String myString = String(currentValue);
  //Lo enviamos por el puerto serie para debug
  Serial.println(myString);
  //Creamos una std::string vacío
  std::string myStdString="";
  //Recorremos todos los caracteres del String y lo añadimos uno a uno al std::string
  for(byte i=0;i<myString.length();i++) myStdString+=myString[i];
  //Escribimos el std::string como valor de la característica
  pCharacteristic->setValue(myStdString);
}

void loop()
{
  //Si hay algún cliente conectado...
  if(clientsCount>0)
  {
    //Actualizamos el valor
    valueUpdate();
    //Enviamos una notificación a cualquier cliente que esté conectado
    pCharacteristic->notify();
    //Esperamos un momento
    delay(1000);
  }
}
