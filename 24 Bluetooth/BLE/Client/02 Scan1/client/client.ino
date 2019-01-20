#include <Arduino.h>
#include <BLEDevice.h> //Gestión de dispositivos

const std::string deviceName="ESP32"; //Nombre del dispositivo a buscar
const BLEUUID advertisedId=BLEUUID("4FAFC201-1FB5-459E-8FCC-C5C9C331914B"); //Identificador del dispositivo a buscar
bool found; //Se ha encontrado?
int myRSSI; //Intensidad de señal


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
            
            //Anotamos la intensidad de la señal
            myRSSI=advertisedDevice.getRSSI();
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
  //Inicializamos BLE sin asignar un nombre (porque es un cliente)
  BLEDevice::init("");
  //Anotamos el scan asociado a nuestro dispositivo
  BLEScan* pBLEScan = BLEDevice::getScan();
  //Asignamos nuestra propia clase que gestiona las llamadas del scan
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  //Activamos el modo de scan activo. Consume algo más de energía, pero es más rápido
  pBLEScan->setActiveScan(true);
}

void loop()
{
  //Anotamos la hora actual
  uint32_t startTime=millis();
  //Todavía no hemos encontrado nada
  found=false;
  //Lanzamos un scan de duración determinada
  BLEDevice::getScan()->start(4);
  //Informamos del tiempo transcurido
  Serial.print(String(millis()-startTime)+" ms : ");
    //Si lo hemos encontrado...
  if(found) Serial.println("RSSI: "+String(myRSSI));
  else Serial.println("no encontrado");
}
