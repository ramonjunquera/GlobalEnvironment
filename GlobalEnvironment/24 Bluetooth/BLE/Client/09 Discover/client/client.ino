#include <Arduino.h>
#include <BLEDevice.h> //Gestión de dispositivos
#include "RoJoSwitch.h" //Gestión de interruptores

//Identificador de la característica
const std::string deviceName="ESP32"; //Nombre del dispositivo a buscar
bool found=false; //Se ha encontrado?. Inicialmente no
BLEAddress *pServerAddress; //Puntero para guardar la dirección del servidor (MAC address)
BLERemoteCharacteristic* pMyRemoteCharacteristic; //Puntero con la característica buscada
byte currentStatus=0; //Comenzamos con el led apagado. 0=apagado, 1=encendido
const byte LED_BUILTIN=2; //Led integrado de ESP32 OLED TTGO 16Mb
const byte pinSwitch=0; //Pin asignado a interruptor integrado en ESP32 OLED TTGO 16Mb
RoJoSwitch mySwitch(pinSwitch); //Objeto de gestión del interruptor

class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks
{
  //Creamos nuestra propia clase heredada de la que gestiona las llamadas a las publicaciones
  //de los dispositivos.
  //La clase original sólo tiene dos métodos: un destructor y onResult que es llamada cuando
  //se detecta la publicación de un nuevo dispositivo
  void onResult(BLEAdvertisedDevice advertisedDevice)
  {
    //Se ha detectado un nuevo dispositivo

    //Comprobamos si tiene el nombre requerido
    if(advertisedDevice.haveName())
    {
      if(advertisedDevice.getName().c_str()==deviceName)
      {
        Serial.println("Encontrado dispositivo: "+String(deviceName.c_str()));
        Serial.println("Identificador de publicación: "+String(advertisedDevice.getServiceUUID().toString().c_str()));
        //Anotamos la dirección del servidor (MAC)
        pServerAddress = new BLEAddress(advertisedDevice.getAddress());
        //Anotamos que lo hemos encontrado
        found=true;
        //Detenemos el scan
        advertisedDevice.getScan()->stop();            
      }
    }
  }
};

void setup()
{
  Serial.begin(115200);
  
  //Configuramos el led integrado como salida
  pinMode(LED_BUILTIN,OUTPUT);
  //Asignamos el estado inicial
  digitalWrite(LED_BUILTIN,currentStatus);
  
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
  //Se ha encontrado el dispositivo buscado

  //Creamos un nuevo objeto de conexión cliente
  BLEClient* pClient=BLEDevice::createClient();
  //Conectamos el cliente con el servidor encontrado a través de la dirección MAC
  pClient->connect(*pServerAddress);

  //Primero mostraremos todos los servicios que encontremos en el servidor, junto
  //con sus características

  //Obtenemos todos los servicios remotos que ofrece
  std::map<std::string, BLERemoteService*>* pRemoteServices=pClient->getServices();
  //Informamos del número de servicios detectados
  Serial.println("Servicios detectados "+String(pRemoteServices->size()));
  //Recorremos todos los servicios detectados
  for(auto &itrRemoteServices : *pRemoteServices)
  {
    //Mostramos el identificador del servicio procesado
    //Corresponde con el primer elemento del mapa (la clave)
    Serial.println("Servicio: "+String(itrRemoteServices.first.c_str()));
    //Anotamos el puntero al objeto de gestión del servicio remoto
    //Corresponde con el segundo elemento del mapa (los datos)
    BLERemoteService* pRemoteService=itrRemoteServices.second;
    //Obtenemos todas las características del servicio remoto procesado
    std::map<std::string, BLERemoteCharacteristic*>* pRemoteCharacteristic=pRemoteService->getCharacteristics();
    //Informamos del número de características que ofrece
    Serial.println("  Características detectadas "+String(pRemoteCharacteristic->size()));
    //Recorremos todas las características detectadas
    for(auto &itrRemoteCharacteristic : *pRemoteCharacteristic)
    {
      //Mostramos el identificador de la característica procesada
      //Corresponde con el primer elemento del mapa (la clave)
      Serial.println("  Característica: "+String(itrRemoteCharacteristic.first.c_str()));
      //Anotamos el puntero al objeto de gestión de la característica remota
      //Corresponde con el segundo elemento del mapa (los datos) 
      BLERemoteCharacteristic* pRemoteCharacteristic=itrRemoteCharacteristic.second;
      //Mostramos sus propiedades
      Serial.print("    Propiedades:");
      if(pRemoteCharacteristic->canRead()) Serial.print(" READ");
      if(pRemoteCharacteristic->canWrite()) Serial.print(" WRITE");
      if(pRemoteCharacteristic->canNotify()) Serial.print(" NOTIFY");
      if(pRemoteCharacteristic->canIndicate()) Serial.print(" INDICATE");
      Serial.println();
    }
  }

  //Ahora anotaremos la última característica que encontremos que no sea genérica y que no
  //pertenezca a un servicio genérico
  //En nuestro ejemplo, sólo tenemos una

  //Inicialmente no hemos encontrado una característica válida
  found=false;
  Serial.println();
  //Ya tenemos de antes el mapa con los servicios remotos en pRemoteServices
  //Recorremos todos los servicios detectados
  for(auto &itrRemoteServices : *pRemoteServices)
  {
    //Si el identificador del servicio no comienza con 4 ceros (si no es genérico)...
    if(itrRemoteServices.first.substr(0,4)!="0000")
    {
      //Anotamos el puntero al objeto de gestión del servicio remoto
      //Corresponde con el segundo elemento del mapa (los datos)
      BLERemoteService* pRemoteService=itrRemoteServices.second;
      //Obtenemos todas las características del servicio remoto procesado
      std::map<std::string, BLERemoteCharacteristic*>* pRemoteCharacteristic=pRemoteService->getCharacteristics();
      //Recorremos todas las características detectadas
      for(auto &itrRemoteCharacteristic : *pRemoteCharacteristic)
      {
        //Si el identificador de la característica no comienza con 4 ceros (si no es genérica)...
        if(itrRemoteCharacteristic.first.substr(0,4)!="0000")
        {
          //...hemos encontrado una característica válida!
          found=true;
          //Anotamos el puntero al objeto de gestión de la característica remota
          //Corresponde con el segundo elemento del mapa (los datos) 
          BLERemoteCharacteristic* pRemoteCharacteristic=itrRemoteCharacteristic.second;
          //Anotamos esta característica en la variable global para tenerla disponible
          pMyRemoteCharacteristic=pRemoteCharacteristic;
          //Informamos
          Serial.println("Detectada característica "+String(pRemoteCharacteristic->getUUID().toString().c_str()));
          Serial.println("de servicio: "+String(pRemoteService->getUUID().toString().c_str()));
        }
      }
    }
  }

  //Si no hemos encontrado una característica válida...
  if(!found)
  {
    Serial.println("No se ha encontrado una característica no genérica");
    while(1);
  }
  //Hemos encontrado una característica válida
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
    //Escribimos el valor del estado en la característica BLE
    //La definición del método es:
    //void writeValue(uint8_t newValue, bool response = false);
    pMyRemoteCharacteristic->writeValue(currentStatus);
  }
}
