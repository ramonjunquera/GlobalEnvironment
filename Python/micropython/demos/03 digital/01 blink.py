#Autor: Ramón Junquera
#Fecha: 20221231

#Demostración de cómo se configura un pin de salida
#Tomamos uno que tiene asociado un led integrado
#El estado del led es inverso al del pin. pin HIGH = led OFF
#Comenzamos encendiendo el led un instante para después entrar en un
#bucle infinito de parpadeo

#DEVICE_ID="ESP-12E"
DEVICE_ID="LOLIN32"

from machine import Pin
from time import sleep

if DEVICE_ID=="ESP-12E":
    pinLed=16
elif DEVICE_ID=="LOLIN32":
    pinLed=5

led=Pin(pinLed,Pin.OUT)

led.off() #Encendemos
sleep(0.5)
led.on() #Apagamos
sleep(2)

while True:
    led.value(not led.value())
    sleep(1)

