#Autor: Ramón Junquera
#Fecha: 20230103
#Demostración de uso de módulo RoJoNeoPixel1x1 para gestión de un
#sólo led Neopixel

#Para pasar de un color a otro, debemos pulsar el botón

#Módulos necesarios: RoJoNeoPixel1x1.py, RoJoSwitch.py

DEVICE_ID="ATOM_LITE"

from RoJoNeoPixel1x1 import RoJoNeoPixel1x1
from RoJoSwitch import RoJoSwitch
import machine #Para idle

if DEVICE_ID=="ATOM_LITE":
    pinNeoPixel=27
    pinSwitch=39

display=RoJoNeoPixel1x1(pinNeoPixel)
button=RoJoSwitch(39)

def waitButton(): #Esperamos a que se pulse el botón
    while not button.pressed():
        machine.idle()

while True:
    display.drawPixel((50,50,50)) #Blanco
    waitButton()
    display.drawPixel((50,0,0)) #Rojo
    waitButton()
    display.drawPixel((0,50,0)) #Verde
    waitButton()
    display.drawPixel((0,0,50)) #Azul
    waitButton()
    display.drawPixel((50,50,0)) #Amarillo
    waitButton()
    display.drawPixel((50,0,50)) #Magenta
    waitButton()
    display.drawPixel((0,50,50)) #Cyan
    waitButton()
    display.drawPixel((0,0,0)) #Negro
    waitButton()
