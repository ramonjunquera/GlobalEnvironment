#include <Arduino.h>
#include <BLEDevice.h> //Gestión de dispositivos
#include <BLEServer.h> //Gestión de servidores

//Constantes globales
const byte LED_BUILTIN=22; //Led integrado de Wemos Lolin32 Lite

//Variables globales
BLECharacteristic *pCharacteristic; //Caracteristica BLE. Necesaria para asignar su valor

//Creamos una clase heredada de la que gestiona las llamadas de una característica
//La adaptaremos para gestionar las peticiones de escritura
class MyCharacteristicCallbacks: public BLECharacteristicCallbacks
{
  //Reescribimos el método que escribe el valor
  //Este método es llamado justo después de recibir un nuevo valor y guardarlo en
  //la característica
  void onWrite(BLECharacteristic *pCharacteristic)
  {
    //Anotamos el valor recibido
    //Sólo existe un método para ello y devuelve un std::string
    std::string rxString=pCharacteristic->getValue();
    //Puesto que nosotros recibimos un byte, sólo nos interesa el primero de los
    //caracteres del String
    byte rxValue=rxString[0];
    //El valor recibido debe ser 0 o 1
    //Si el valor recibido es mayor que 1...lo tomaremos como 1
    if(rxValue>1) rxValue=1;
    //Aplicaremos el estado recibido al led integrado
    //Puesto que la placa es una Wemos Lolin32 Lite, los estados del led integrado
    //son inversos a los de su pin asociado
    digitalWrite(LED_BUILTIN,1-rxValue);
  }
};

void setup()
{
  Serial.begin(115200);

  //Configuramos el led como salida
  pinMode(LED_BUILTIN,OUTPUT);
  //Inicialmente apagado
  digitalWrite(LED_BUILTIN,HIGH);
  
  //Asignamos un nombre descriptivo al dispositivo
  //Para guardar compatibilidad intentaremos que siempre tenga un máximo de 5 caracteres
  BLEDevice::init("ESP32");
  //Creamos un servidor dentro del dispositivo
  BLEServer *pServer = BLEDevice::createServer();
  //Creamos un servicio en el servidor al que asignamos un identificador
  BLEService *pService = pServer->createService("4FAFC201-1FB5-459E-8FCC-C5C9C331914B");
  //Creamos una característica para el servicio a la que le asignamos un identificador
  //También indicamos qué tipo de acceso tendrá (propiedad). En este sólo se podrá
  //escribir
  pCharacteristic = pService->createCharacteristic("BEB5483E-36E1-4688-B7F5-EA07361B26A8",BLECharacteristic::PROPERTY_WRITE);
  //Aplicamos a la característica nuestro propio sistema de gestión de llamadas definido
  //anteriormente que se encargue de las escrituras del valor (modificaciones)
  pCharacteristic->setCallbacks(new MyCharacteristicCallbacks());
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

void loop()
{
  //Nada especial que hacer aquí
}
