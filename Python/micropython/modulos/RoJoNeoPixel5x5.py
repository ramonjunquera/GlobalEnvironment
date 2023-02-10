#Autor: Ramón Junquera
#Fecha: 20230103
#Driver de video para matriz de NeoPixel de 5x5 de M5 Atom Matrix

from machine import Pin
from neopixel import NeoPixel

class RoJoNeoPixel5x5:
    def __init__(self,pin):
        self._videoMem=NeoPixel(Pin(pin,Pin.OUT),25)
    #Convierte coordenadas x,y a índice
    #  p : tupla/lista de coordenadas x,y del pixel [0,4]
    def _xy2index(self,p):
        return 5*p[1]+p[0]
    #Asigna color a un pixel
    #  p : tupla/lista de coordenadas x,y del pixel [0,4]
    #  color : tupla/lista RGB con color
    def drawPixel(self,p,color):
        self._videoMem[self._xy2index(p)]=color
    #Transmite memoria de video a display
    def show(self):
        self._videoMem.write()
    #Pinta la pantalla de un color. Si se omite, negro
    #  color : tupla/lista RGB con color
    def clear(self,color=(0,0,0)):
        for i in range(25):
            self._videoMem[i]=color
    #Obtiene el color de un pixel
    #  p : tupla/lista de coordenadas x,y del pixel [0,7]
    #Devuelve tupla/lista RGB de color
    def getPixel(self,p):
        return self._videoMem[self._xy2index(p)]

