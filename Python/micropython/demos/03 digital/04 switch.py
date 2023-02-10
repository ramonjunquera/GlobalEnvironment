#Autor: Ramón Junquera
#Fecha: 20221231
#Demostración de uso de módulo RoJoSwitch

#Módulos necesarios: RoJoSwitch.py

#Importamos la clase RoJoSwitch del módulo RoJoSwitch
#Instanciamos en el objeto switch para el pin asociado al botón FLASH en ESP-12E
#En un bucle infinito, mostramos los distintos estados

#DEVICE_ID="ESP-12E"
DEVICE_ID="LOLIN32"

from RoJoSwitch import RoJoSwitch
import time #Para sleep

if DEVICE_ID=="ESP-12E":
    pinSwitch=0 #D3
elif DEVICE_ID=="LOLIN32":
    pinSwitch=22

switch=RoJoSwitch(pinSwitch) #Instanciamos un interruptor

while True:
    if switch.pressed():
        print("pulsado")
    if switch.released():
        print("soltado")
    print("pulsando",switch.pressing())
    time.sleep(0.3)

