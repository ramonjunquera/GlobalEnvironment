#Autor: Ramón Junquera
#Fecha: 20230103
#Driver de video para un sólo led NeoPixel

from machine import Pin
from neopixel import NeoPixel

class RoJoNeoPixel1x1:
    def __init__(self,pin):
        self._videoMem=NeoPixel(Pin(pin,Pin.OUT),1)
    #Asigna color al pixel
    #  color : tupla/lista RGB con color
    def drawPixel(self,color):
        self._videoMem[0]=color
        self._videoMem.write()
    #Obtiene el color del pixel
    #Devuelve tupla/lista RGB de color
    def getPixel(self,p):
        return self._videoMem[0]


