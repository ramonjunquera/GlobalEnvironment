#include <Arduino.h>
#include <BLEDevice.h> //Gestión de dispositivos

class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks
{
  //Creamos nuestra propia clase heredada de la que gestiona las llamadas a las publicaciones
  //de los dispositivos.
  //La clase original sólo tiene dos métodos: un destructor y onResult que es llamada cuando
  //se detecta la publicación de un nuevo dispositivo
  void onResult(BLEAdvertisedDevice advertisedDevice)
  {
    //Se ha detectado un nuevo dispositivo

    //Simplemente informaremos por el puerto serie de todos los detalles recopilados
    Serial.println(advertisedDevice.toString().c_str());
    //advertisedDevice.toString() contiene toda la información separada por comas
    //Pero se pueden obtener cada uno de los detalles por separado
    //Serial.println("MAC: "+String(advertisedDevice.getAddress().toString().c_str())); //MAC address
    //if(advertisedDevice.haveName()) Serial.println("Nombre: "+String(advertisedDevice.getName().c_str())); //Nombre del dispositivo
    //if(advertisedDevice.haveTXPower()) Serial.println("TXPower: "+String(advertisedDevice.getTXPower())); //Potencia de transmisión
    //if(advertisedDevice.haveRSSI()) Serial.println("RSSI: "+String(advertisedDevice.getRSSI())); //Intensidad de señal (RSSI)
  }
};

void setup()
{
  Serial.begin(115200);
  Serial.println("\nScan iniciado");
  
  //Asignamos un nombre descriptivo al dispositivo
  //En clientes no es necesario. Podríamos asignar un nombre vacío ""
  BLEDevice::init("myClient");
  //Creamos un objeto para hacer el scan
  BLEScan* pBLEScan = BLEDevice::getScan();
  //Asignamos nuestra propia clase que gestiona las llamadas del scan
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  //Activamos el modo de scan activo. Consume algo más de energía, pero es más rápido
  pBLEScan->setActiveScan(true);
  //Pedimos que se haga un scan de 15 segundos y anotamos los disposivos encontrados
  BLEScanResults foundDevices = pBLEScan->start(15);
  //Mostramos el número de dispositivos encontrados
  Serial.println("Dispositivos encontrados: "+String(foundDevices.getCount()));
}

void loop()
{
  //Nada especial que hacer aquí
}
