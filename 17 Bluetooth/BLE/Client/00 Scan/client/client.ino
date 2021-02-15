#include <Arduino.h>
#include <BLEDevice.h> //Gestión de dispositivos

void setup()
{
  Serial.begin(115200);
  Serial.println("\nScan iniciado");
  
  //Asignamos un nombre descriptivo al dispositivo
  //En clientes no es necesario. Podríamos asignar un nombre vacío ""
  BLEDevice::init("myClient");
  //Creamos un objeto para hacer el scan
  BLEScan* pBLEScan = BLEDevice::getScan();
  //Activamos el modo de scan activo. Consume algo más de energía, pero es más rápido
  pBLEScan->setActiveScan(true);
  //Pedimos que se haga un scan de 15 segundos y anotamos los disposivos encontrados
  BLEScanResults foundDevices = pBLEScan->start(15);
  //Mostramos el número de dispositivos encontrados
  Serial.println("Dispositivos encontrados: "+String(foundDevices.getCount()));

  //Recorremos todos los dispositivos detectados en el scan
  for(int deviceIndex=0;deviceIndex<foundDevices.getCount();deviceIndex++)
  {
    //Anotamos el dispositivo
    BLEAdvertisedDevice myAdvertisedDevice=foundDevices.getDevice(deviceIndex);
    //Simplemente informaremos por el puerto serie de todos los detalles recopilados
    //poniendo como prefijo el contador del dispositivo detectado
    Serial.println(String(deviceIndex+1)+" : " + String(myAdvertisedDevice.toString().c_str()));
    //myAdvertisedDevice.toString() contiene toda la información separada por comas
    //Pero se pueden obtener cada uno de los detalles por separado
    //Serial.println("MAC: "+String(myAdvertisedDevice.getAddress().toString().c_str())); //MAC address
    //if(myAdvertisedDevice.haveName()) Serial.println("Nombre: "+String(myAdvertisedDevice.getName().c_str())); //Nombre del dispositivo
    //if(myAdvertisedDevice.haveTXPower()) Serial.println("TXPower: "+String(myAdvertisedDevice.getTXPower())); //Potencia de transmisión
    //if(myAdvertisedDevice.haveRSSI()) Serial.println("RSSI: "+String(myAdvertisedDevice.getRSSI())); //Intensidad de señal (RSSI)
  }
}

void loop()
{
  //Nada especial que hacer aquí
}
