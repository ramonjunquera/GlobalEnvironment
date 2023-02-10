#Autor: Ramón Junquera
#Fecha: 20221231
#Demostración de uso de módulo RoJoSSD1331

#Módulos necesarios: RoJoSSD1331.py

#DEVICE_ID="ESP-12E"
DEVICE_ID="LOLIN32"

from RoJoSSD1331 import RoJoSSD1331
import time #Para sleep

if DEVICE_ID=="ESP-12E":
    pinDC=4 #D2
    pinRES=16 #D0
    pinCS=15 #D8
    spiID=1
elif DEVICE_ID=="LOLIN32":
    pinDC=22
    pinRES=2
    pinCS=5
    spiID=2
   
def test1(): #Test de color
    print("test1")
    display.clear()
    sizeT=(32,32) #Todos los bloques serán de igual tamaño
    display.block((0,0),sizeT,(255,0,0)) #Rojo
    display.block((32,0),sizeT,(0,255,0)) #Verde
    display.block((64,0),sizeT,(0,0,255)) #Azul
    display.block((0,32),sizeT,(255,255,0)) #Amarillo
    display.block((32,32),sizeT,(255,0,255)) #Magenta
    display.block((64,32),sizeT,(0,255,255)) #Cyan
    time.sleep(1)
def test2(): #Test de pixels
    print("test2")
    display.clear()
    #Dibujamos una matriz de puntos
    for y in range(0,display.size[1],5):
        for x in range(0,display.size[0],5):
            display.drawPixel((x,y),(x*2,y*4,0))
    time.sleep(1)
def test3(): #Test de lineas
    print("test3")
    display.clear()
    #Dibujamos líneas verdes
    color=(0,255,0) #verde
    for x in range(0,display.size[0],10):
        display.line((0,0),(x,display.size[1]-1),color)
    #Dibujamos líneas azules
    color=(0,0,255) #azul
    for x in range(0,display.size[0],10):
        display.line((0,display.size[1]-1),(x,0),color)
    time.sleep(1)
def test4(): #Test de rectángulos
    print("test4")
    display.clear()
    borderColor=(255,255,255) #white
    fillColor=(0,0,255) #blue
    #Dibujamos una matriz de 5x5 rectángulos con borde y relleno
    for x in range(0,50,10):
        for y in range(0,50,10):
            display.rectFill((x,y),(9,9),borderColor,fillColor)
    #Dibujamos un rectángulo rojo sin relleno sobre lo anterior
    #para comprobar que el relleno es transparente
    display.rect((15,15),(20,20),(255,0,0))
    time.sleep(1)
def test5(): #Test de triángulos
    print("test5")
    display.clear()
    display.triangle((20,0),(50,40),(0,20),(255,0,0)) #Rojo
    display.triangle((20,40),(80,10),(40,60),(0,255,0)) #Verde
    display.triangle((50,10),(90,30),(70,60),(0,0,255)) #Azul
    time.sleep(1)
def test7(): #Test darker
    print("test7")
    display.clear()
    #Dibujamos un rectángulo de borde blanco y relleno azul
    display.rectFill((0,0),(50,50),(255,255,255),(0,0,255))
    #Oscurecemos dos areas que se solapan
    display.darker((10,10),(20,20))
    display.darker((20,20),(20,20))
    #Comprobamos que una zona oscurecida ya no se oscurece más
    time.sleep(1)
def test8(): #Test de copia
    print("test8")
    display.clear()
    #Dibujamos un rectángulo de borde blanco y relleno azul
    display.rectFill((0,0),(8,8),(255,255,255),(0,0,255))
    #Lo copiamos 5 veces
    for i in range(5):
        display.copy((0,0),(8,8),(i*10,10))
    time.sleep(1)
   
display=RoJoSSD1331(pinRES,pinDC,pinCS,spiID)
test1()
test2()
test3()
test4()
test5()
test7()
test8()
