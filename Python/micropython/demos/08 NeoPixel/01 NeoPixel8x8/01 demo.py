#Autor: Ramón Junquera
#Fecha: 20230103
#Demostración de uso de módulo RoJoNeoPixel8x8 para gestión de matriz de
#leds Neopixel de 8x8 CJMCU-64

#Módulos necesarios: RoJoNeoPixel8x8.py

#DEVICE_ID="ESP-12E"
DEVICE_ID="LOLIN32"

from RoJoNeoPixel8x8 import RoJoNeoPixel8x8
import time #Para sleep

if DEVICE_ID=="ESP-12E":
    pin=0 #D3
elif DEVICE_ID=="LOLIN32":
    pin=13

display=RoJoNeoPixel8x8(pin) #Instanciamos driver

redColor=(30,0,0) #Rojo
greenColor=(0,30,0) #Verde

display.clear() #Pantalla en negro
for i in range(8):
    display.drawPixel((i,i),redColor) #Diagonal en rojo
    display.drawPixel((7-i,i),greenColor) #Diagonal en verde
display.show()
time.sleep(1)

for y in range(8):
    for x in range(8):
        display.drawPixel((x,y),(4+x*10,1+y*10,0))
display.show()
time.sleep(1)

for y in range(8):
    for x in range(8):
        p=(x,y)
        color=display.getPixel(p)
        #Intercambiamos los canales verde y azul
        color=(color[0],color[2],color[1])
        display.drawPixel(p,color)
display.show()
time.sleep(1)

