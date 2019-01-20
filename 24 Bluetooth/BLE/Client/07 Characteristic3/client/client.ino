#include <Arduino.h>
#include <BLEDevice.h> //Gestión de dispositivos
#include "RoJoSwitch.h" //Gestión de interruptores

//Constantes globales
const byte pinLed[]={26,14,13}; //Pines de los leds
const byte pinSwitch[]={25,27,12}; //Pines de los interruptores
const BLEUUID advertisedId=BLEUUID("4FAFC201-1FB5-459E-8FCC-C5C9C331914B"); //Id de publicación y servicio
const std::string characteristicId[]= //Identificadores del características
{
  "BEB5483E-36E1-4688-B7F5-EA07361B26A0",
  "BEB5483E-36E1-4688-B7F5-EA07361B26A1",
  "BEB5483E-36E1-4688-B7F5-EA07361B26A2"
};

//Variables globales

BLERemoteCharacteristic* pRemoteCharacteristics[3]; //Características
byte currentStatus[3]; //Estados actuales
const std::string deviceName="ESP32"; //Nombre del dispositivo a buscar
bool found=false; //Se ha encontrado?. Inicialmente no
BLEAddress *pServerAddress; //Puntero para guardar la dirección del servidor (MAC address)
RoJoSwitch switches[3]; //Objetos de gestión de los interruptores

class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks
{
  //Creamos nuestra propia clase heredada de la que gestiona las llamadas a las publicaciones
  //de los dispositivos.
  //La clase original sólo tiene dos métodos: un destructor y onResult que es llamada cuando
  //se detecta la publicación de un nuevo dispositivo
  void onResult(BLEAdvertisedDevice advertisedDevice)
  {
    //Se ha detectado un nuevo dispositivo

    if(advertisedDevice.haveName())
    {
      if(advertisedDevice.getName().c_str()==deviceName)
      {
        if(advertisedDevice.haveServiceUUID())
        {
          if(advertisedDevice.getServiceUUID().equals(BLEUUID(advertisedId)))
          {
            //Hemos localizado el dispositivo buscado!
            
            //Anotamos la dirección del servidor (MAC)
            pServerAddress = new BLEAddress(advertisedDevice.getAddress());
            //Anotamos que lo hemos encontrado
            found=true;
            //Detenemos el scan
            advertisedDevice.getScan()->stop();
          }
        }
      }
    }
  }
};

void setup()
{
  Serial.begin(115200);

  //Configuramos los pines de los leds como salidas, los apagamos y fijamos su estado a 0
  for(byte i=0;i<3;i++)
  {
    pinMode(pinLed[i],OUTPUT);
    digitalWrite(pinLed[i],LOW);
    currentStatus[i]=0;
  }
  //Creamos los objetos de gestión de los interruptores
  for(byte i=0;i<3;i++)
  {
    switches[i]=RoJoSwitch(pinSwitch[i]);
  }
  
  //Inicializamos BLE sin asignar un nombre (porque es un cliente)
  BLEDevice::init("");
  //Anotamos el scan asociado a nuestro dispositivo
  BLEScan* pBLEScan = BLEDevice::getScan();
  //Asignamos nuestra propia clase que gestiona las llamadas del scan
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  //Activamos el modo de scan activo. Consume algo más de energía, pero es más rápido
  pBLEScan->setActiveScan(true);
  //Repetiremos el scan hasta encontrar el dispositivo buscado
  do
  {
    Serial.println("\nScan iniciado");
    //Lanzamos un scan de duración determinada
    BLEDevice::getScan()->start(5);
  }
  while(!found);
  Serial.println("Dispositivo encontrado");

  //Creamos un nuevo objeto de conexión cliente
  BLEClient* pClient=BLEDevice::createClient();
  //Conectamos el cliente con el servidor encontrado
  pClient->connect(*pServerAddress);
  //Creamos un nuevo objeto de gestión de un servicio remoto con el identificador
  //del servicio del servidor encontrado
  //El identificador de servicio no tiene porqué coincidir con el identificador
  //de publicación (aunque habitualmente sea así).
  BLERemoteService* pRemoteService=pClient->getService(advertisedId);
  //Si no encontramos un servicio en el servidor...
  if(pRemoteService==nullptr)
  {
    Serial.print("No se encuentra el servicio del servidor");
    while(1); //Bucle infinito
  }
  Serial.println("Servicio encontrado");

  //Creamos los objetos de gestión de las características remotas
  for(byte i=0;i<3;i++)
  {
    pRemoteCharacteristics[i]=pRemoteService->getCharacteristic(characteristicId[i]);
    //Si no encontramos la característica en el servicio...
    if(pRemoteCharacteristics[i]==nullptr)
    {
      Serial.print("No se encuentra la característica "+String(i));
      while(1); //Bucle infinito
    }
    Serial.println("Encontrada característica "+String(i));
  }
}

void loop()
{
  //Recorremos los 3 interruptores
  for(byte i=0;i<3;i++)
  {
    //Si se ha pulsado el botón...
    if(switches[i].pressed())
    {
      //...cambiamos el estado actual
      currentStatus[i]=1-currentStatus[i];
      //Aplicamos el nuevo estado al led
      digitalWrite(pinLed[i],currentStatus[i]);
      //Escribimos el valor del estado en la característica BLE
      //La definición del método es:
      //void writeValue(uint8_t newValue, bool response = false);
      pRemoteCharacteristics[i]->writeValue(currentStatus[i]);
    }
  }
}
