#include <Arduino.h>
#include <BLEDevice.h> //Gestión de dispositivos
#include <BLEServer.h> //Gestión de servidores

//Constantes globales
const byte pinLed[]={27,14,12}; //Pines de los leds
const std::string servicesId[]= //Identificadores de los servicios
{
  "4FAFC201-1FB5-459E-8FCC-C5C9C3319140",
  "4FAFC201-1FB5-459E-8FCC-C5C9C3319141",
  "4FAFC201-1FB5-459E-8FCC-C5C9C3319142"
};
const std::string characteristicId[]= //Identificadores de las características
{
  "BEB5483E-36E1-4688-B7F5-EA07361B26A0",
  "BEB5483E-36E1-4688-B7F5-EA07361B26A1",
  "BEB5483E-36E1-4688-B7F5-EA07361B26A2"
};
const std::string advertisingId="BEB5483E-36E1-4688-B7F5-EA07361B26AA"; //Identificador de publicación

//Variables globales
BLECharacteristic *pCharacteristics[3]; //Características

void setStatus(std::string rxString,byte indexLed)
{
  //Procesa el valor recibido y lo aplica a un led

  //Extraemos el primero de los bytes (que es el que tiene el valor)
  byte rxValue=rxString[0];
  //El valor recibido debe ser 0 o 1
  //Si el valor recibido es mayor que 1...lo tomaremos como 1
  if(rxValue>1) rxValue=1;
  //Aplicaremos el estado recibido al led indicado
  digitalWrite(pinLed[indexLed],rxValue);  
}

//Creamos una clase heredada de la que gestiona las llamadas de una característica
//para cada una de las 3 características.
//La adaptaremos para gestionar las peticiones de escritura
class MyCharacteristicCallbacks0: public BLECharacteristicCallbacks
{
  //Reescribimos el método que escribe el valor
  //Este método es llamado justo después de recibir un nuevo valor y guardarlo en
  //la característica
  void onWrite(BLECharacteristic *pCharacteristic)
  {
    //Enviamos el valor recibido para que se aplique a un determinado led
    setStatus(pCharacteristic->getValue(),0);
  }
};
class MyCharacteristicCallbacks1: public BLECharacteristicCallbacks
{
  void onWrite(BLECharacteristic *pCharacteristic)
  {
    setStatus(pCharacteristic->getValue(),1);
  }
};
class MyCharacteristicCallbacks2: public BLECharacteristicCallbacks
{
  void onWrite(BLECharacteristic *pCharacteristic)
  {
    setStatus(pCharacteristic->getValue(),2);
  }
};

void setup()
{
  Serial.begin(115200);

  //Configuramos los pines de los leds como salidas y los apagamos
  for(byte i=0;i<3;i++)
  {
    pinMode(pinLed[i],OUTPUT);
    digitalWrite(pinLed[i],LOW);
  }
 
  //Asignamos un nombre descriptivo al dispositivo
  //Para guardar compatibilidad intentaremos que siempre tenga un máximo de 5 caracteres
  BLEDevice::init("ESP32");
  //Creamos un servidor dentro del dispositivo
  BLEServer *pServer = BLEDevice::createServer();

  //Creamos los 3 servicios, cada una de sus características, asignamos la gestión de llamadas y lo iniciamos
  for(byte i=0;i<3;i++)
  {
    //Creamos el servicio
    BLEService *pService = pServer->createService(servicesId[i]);
    //Creamos la característica con su corresponsiente identificador y propiedades
    pCharacteristics[i]=pService->createCharacteristic(characteristicId[i],BLECharacteristic::PROPERTY_WRITE);
    //Asignamos nuestra gestión de llamadas
    switch(i)
    {
      case 0:
        pCharacteristics[0]->setCallbacks(new MyCharacteristicCallbacks0());
        break;
      case 1:
        pCharacteristics[1]->setCallbacks(new MyCharacteristicCallbacks1());
        break;
      case 2:
        pCharacteristics[2]->setCallbacks(new MyCharacteristicCallbacks2());
        break;
    }
    //Arrancamos el servicio
    pService->start();
  }
  
  //El servidor podrá ser publicado y visible
  BLEAdvertising *pAdvertising = pServer->getAdvertising();
  //Añadimos el número de identificación del servicio para que sea publicado y visible
  pAdvertising->addServiceUUID(advertisingId);
  //Iniciamos la publicación del servidor
  pAdvertising->start();
  //Informamos que ya está disponible el servicio BLE
  Serial.println("\nBLE iniciado");  
}

void loop()
{
  //Nada especial que hacer aquí
}
