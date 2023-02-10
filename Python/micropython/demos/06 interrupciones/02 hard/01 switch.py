#Autor: Ramón Junquera
#Fecha: 20221231
#Interrupciones hardware

#Módulos necesarios: RoJoSwitch.py

#Utilizamos el módulo RoJoSwitch para simplificar la gestión del interruptor integrado
#Crearemos una interrupción en el pin del pulsador para que
#cuando se detecte un cambio de estado, se llame al método que
#comprueba que el cambio de estado corresponde a una pulsación
#y cambie el estado el led

#La ventaja es que no hay que estar continuamente comprobando el 
#estado del interruptor (polling)

#DEVICE_ID="ESP-12E"
DEVICE_ID="LOLIN32"

import machine #Gestión de GPIO
from RoJoSwitch import RoJoSwitch

if DEVICE_ID=="ESP-12E":
    pinSwitch=0 #D3. Pulsador integrado
    pinLed=16 #D0. Led integrado
elif DEVICE_ID=="LOLIN32":
    pinSwitch=22
    pinLed=5 #Led integrado

switch=RoJoSwitch(pinSwitch)
led=machine.Pin(pinLed,machine.Pin.OUT)

#Comprueba que el interruptor ha sido pulsado y cambia
#el estado del led. Es llamada cada vez que se detecta un cambio de
#estado en el pin del interruptor
def check_pressed(p):
    if switch.pressed(): #Si el interruptor se ha pulsado...
        led.value(not led.value()) #...cambiamos el estado del led

#Creamos una interrupción por hardware para el pin del interruptor
#Cuando su estado cambie, llamaremos a la función que decide qué hacer 
#con las pulsaciones del interruptor
switch.pin().irq(trigger=machine.Pin.IRQ_RISING | machine.Pin.IRQ_FALLING,handler=check_pressed)

#El programa termina aquí, pero las interrupciones se siguen ejecutando en background

