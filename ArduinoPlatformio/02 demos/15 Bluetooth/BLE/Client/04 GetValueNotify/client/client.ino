#include <Arduino.h>
#include <BLEDevice.h> //Gestión de dispositivos

//Identificador del dispositivo a buscar, válido para la publicación y para el servicio
const BLEUUID advertisedId=BLEUUID("4FAFC201-1FB5-459E-8FCC-C5C9C331914B");
//Identificador de la característica
const BLEUUID characteristicId=BLEUUID("BEB5483E-36E1-4688-B7F5-EA07361B26A8");
const std::string deviceName="ESP32"; //Nombre del dispositivo a buscar
bool found=false; //Se ha encontrado?. Inicialmente no
BLEAddress *pServerAddress; //Puntero para guardar la dirección del servidor (MAC address)
BLERemoteCharacteristic* pRemoteCharacteristic; //Puntero con la característica
const byte LED_BUILTIN=22; //Pin correspondiente al led integrado en Wemos Lolin32 Lite

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

void notifyCallback(BLERemoteCharacteristic* pBLERemoteCharacteristic,uint8_t* pData,size_t length,bool isNotify)
{
  //Se ha recibido una notificación!

  //Aunque gracias a los parámetros de la función podemos saber el puntero de inicio donde se guarda el
  //valor y el tamaño (número de bytes), ya sabemos que en este caso sólo se envía un byte.
  //Mostramos el valor de la característica que se ha notificado
  Serial.println(*pData);
  //Además aplicamos el mismo estado al led integrado
  //Estamos utilizando un Wemos Lolin32 Lite
  //Esta placa tiene los estados del led inversos a los del pin asociado
  digitalWrite(LED_BUILTIN,1-*pData);
}

void setup()
{
  Serial.begin(115200);
  
  //Configuramos el led integrado como salida
  pinMode(LED_BUILTIN,OUTPUT);
  
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

  //Puesto que el servidor envía notificaciones cuando hay un cambio del valor, indicaremos
  //que cuando se detecte una notificación, se debe llamar a una función propia en la que
  //lo gestionaremos
  pRemoteCharacteristic->registerForNotify(notifyCallback);

  /*
  //De la misma manera que ocurre con la aplicación nRF Connect de Android, no es suficiente 
  //que un servidor tenga notificaciones para que un cliente las reciba automáticamante.
  //En el cliente debemos activar el descriptor remoto 2902
  //Creamos el identificador del descriptor
  BLEUUID descriptor2902Id=BLEUUID((uint16_t)0x2902);
  //Anotamos el objeto de gestión del descriptor remoto 2902
  BLERemoteDescriptor* pDescriptor2902=pRemoteCharacteristic->getDescriptor(descriptor2902Id);
  //Creamos una matriz de 2 bytes que representará el estado de las notificaciones y de las indicaciones.
  //Activaremos las notificaciones y dejaremos desactivadas las indicaciones.
  const byte statusNotifyIndicate[]={1,0};
  //Escribiremos la configuración al descriptor 2902. Esta es la definición del método:
  //void writeValue(uint8_t* data, size_t length, bool response = false);
  pDescriptor2902->writeValue((byte*)statusNotifyIndicate,2,true);
  //Todas las líneas anteriores se pueden resumir en:
  */
  const byte statusNotifyIndicate[]={1,0};
  pRemoteCharacteristic->getDescriptor(BLEUUID((uint16_t)0x2902))->writeValue((byte*)statusNotifyIndicate,2,true);
}

void loop()
{
  //Nada especial que hacer aquí
}
