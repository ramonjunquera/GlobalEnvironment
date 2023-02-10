#Autor: Ramón Junquera
#Fecha: 20221231
#El pulsador integrado cambia el estado del led integrado

#Módulos necesarios: RoJoSwitch.py

#DEVICE_ID="ESP-12E"
DEVICE_ID="LOLIN32"

from RoJoSwitch import RoJoSwitch
from machine import Pin

if DEVICE_ID=="ESP-12E":
    pinSwitch=0 #D3
    pinLed=16 #D0
elif DEVICE_ID=="LOLIN32":
    pinSwitch=22
    pinLed=5

switch=RoJoSwitch(pinSwitch) #Instanciamos un interruptor
led=Pin(pinLed,Pin.OUT) #Configuramos led integrado
while True: #Bucle infinito
 if switch.pressed(): # Si el interruptor se ha pulsado
  led.value(not led.value()) # Intercambiamos el estado del led


