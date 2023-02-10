#Autor: Ramón Junquera
#Fecha: 20230103
#Demostración de uso de módulo RoJoNeoPixel5x5 para gestión de matriz de
#leds Neopixel de 8x8 de M5 Stack Atom Matrix

#Para pasar de un ejemplo a otro, debemos pulsar el botón

#Módulos necesarios: RoJoNeoPixel5x5.py, RoJoSwitch.py

DEVICE_ID="ATOM_MATRIX"

from RoJoNeoPixel5x5 import RoJoNeoPixel5x5
from RoJoSwitch import RoJoSwitch
import machine #Para idle

if DEVICE_ID=="ATOM_MATRIX":
    pinNeoPixel=27
    pinSwitch=39

display=RoJoNeoPixel5x5(pinNeoPixel)
button=RoJoSwitch(39)

redColor=(50,0,0) #Rojo
greenColor=(0,50,0) #Verde

def waitButton(): #Esperamos a que se pulse el botón
    while not button.pressed():
        machine.idle()

while True:
    display.clear() #Pantalla en negro
    for i in range(5):
        display.drawPixel((i,i),redColor) #Diagonal en rojo
        display.drawPixel((4-i,i),greenColor) #Diagonal en verde
    display.show()
    waitButton()
        
    for y in range(5):
        for x in range(5):
            display.drawPixel((x,y),(4+x*10,1+y*10,0))
    display.show()
    waitButton()

    for y in range(5):
        for x in range(5):
            p=(x,y)
            color=display.getPixel(p)
            #Intercambiamos los canales verde y azul
            color=(color[0],color[2],color[1])
            display.drawPixel(p,color)
    display.show()
    waitButton()
    
    display.clear(redColor)
    display.show()
    waitButton()
    
    display.clear(greenColor)
    display.show()
    waitButton()

