#Autor: Ramón Junquera
#Fecha: 20221231

#Demostración de cómo se configura un pin de salida
#Tomamos uno que tiene asociado un led integrado
#El estado del led es inverso al del pin. pin HIGH = led OFF
#Comenzamos encendiendo el led un instante para después entrar en un
#bucle infinito de parpadeo

#Es incómodo que no coincida el estado del pin con el led asociado
#En este ejemplo añadimos una capa con Signal que invierte los estados
#Ahora coincide pin HIGH = led ON

#DEVICE_ID="ESP-12E"
DEVICE_ID="LOLIN32"

from machine import Pin,Signal
from time import sleep

if DEVICE_ID=="ESP-12E":
    pinLed=16
elif DEVICE_ID=="LOLIN32":
    pinLed=5

#Definicmos el pin, su modo e invertimos el estado de su señal
led=Signal(Pin(pinLed,Pin.OUT),invert=True)

led.on() #Encendemos
sleep(0.5)
led.off() #Apagamos
sleep(2)

while True:
    led.value(not led.value())
    sleep(1)

