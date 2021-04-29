#include <Arduino.h>
#include <BLEDevice.h> //Gestión de dispositivos
#include "RoJoSwitch.h" //Gestión de interruptores

//Identificador del dispositivo a buscar, válido para la publicación y para el servicio
const BLEUUID advertisedId=BLEUUID("4FAFC201-1FB5-459E-8FCC-C5C9C331914B");
//Identificador de la característica
const BLEUUID characteristicId=BLEUUID("BEB5483E-36E1-4688-B7F5-EA07361B26A8");
const std::string deviceName="ESP32"; //Nombre del dispositivo a buscar
bool found=false; //Se ha encontrado?. Inicialmente no
BLEAddress *pServerAddress; //Puntero para guardar la dirección del servidor (MAC address)
BLERemoteCharacteristic* pRemoteCharacteristic; //Puntero con la característica
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

  //Creamos un nuevo objeto de gestión de una característica remota con el identificador
  //de la característica, dentro del servicio remoto
  pRemoteCharacteristic=pRemoteService->getCharacteristic(characteristicId);
  //Si no encontramos la característica en el servicio...
  if(pRemoteCharacteristic==nullptr)
  {
    Serial.print("No se encuentra la característica del servicio");
    while(1); //Bucle infinito
  }
  Serial.println("Característica encontrada");
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
    pRemoteCharacteristic->writeValue(currentStatus);
  }
}
