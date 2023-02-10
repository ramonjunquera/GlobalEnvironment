#Autor: Ramón Junquera
#Fecha: 20221231

#Demostración de cómo se configura un pin de entrada con resistencias de PULLUP
#Aprovechamos que en el ESP-12E tenemos el pulsador integrado FLASH, asociado
#al pin D3=0
#Para que la lógica sea más cómoda, invertimos el estado de la señal del pin con Signal
#Ahora cuando se pulse el botón su estado será 1
#Mostramos en un bucle infinito el estado del pin

#DEVICE_ID="ESP-12E"
DEVICE_ID="LOLIN32"

from machine import Pin,Signal
from time import sleep

if DEVICE_ID=="ESP-12E":
    pinSwitch=0 #D3
elif DEVICE_ID=="LOLIN32":
    pinSwitch=22

#Definimos el pin, su modo e invertimos el estado de su señal
switch=Signal(Pin(pinSwitch,Pin.IN,Pin.PULL_UP),invert=True)

while True:
    print(switch.value())
    sleep(0.5)

