#Autor: Ramón Junquera
#Fecha: 20230101
#Demostración de uso de módulo ssd1306

#Este módulo ya viene incluido dentro de MicroPython para ESP8266
#No existe en la versión para ESP32
#Internamente tiene una memoria de video que gestiona el driver
#Siempre es necesario pasar esa memoria al display con el método show()

#Suponemos que el identificador I2C del display es el estandar: 0x3C

DEVICE_ID="ESP-12E"

DISPLAY_SIZE=(128,64) #Tamaño del display

from machine import Pin,I2C
import ssd1306
import time #Para sleep

if DEVICE_ID=="ESP-12E":
    pinSDA=4 #D2
    pinSCL=5 #D1

def test1(): #Borrado de display
    display.fill(1) #Pinta todo el display. Lo llena de 1
    display.show() #Enviamos memoria de video a display
    time.sleep(1)
    display.fill(0) #Borra el display. Lo llena de color 0
    display.show()
    time.sleep(1)
    
def test2(): #Dibuja pixels con color 1
    for y in range(0,DISPLAY_SIZE[1],5):
        for x in range(0,DISPLAY_SIZE[0],5):
            display.pixel(x,y,1) #Activa pixel en color 1
    display.show()
    time.sleep(1)
    
def test3(): #Dibuja pixels con color 0
    for y in range(10,40):
        for x in range(10,40):
            display.pixel(x,y,0) #Pixel a color 0 (negro)
    display.show()
    time.sleep(1)
    
def test4(): #Obtiene color de un pixel
    for y in range(30,50):
        for x in range(30,50):
            color=display.pixel(x,y) #Obtenemos el color del pixel
            color=1-color #Invertimos el color del pixel. 0 <-> 1
            display.pixel(x,y,color)
    display.show()
    time.sleep(1)

def test5(): #Test de rectángulos
    display.fill(0) #Clear
    display.fill_rect(0,0,20,20,1) #Rectángulo relleno. Bloque
    display.rect(10,10,30,30,1) #Rectángulo sin relleno
    display.show()
    time.sleep(1)
        
def test6(): #Tests de lineas
    display.fill(0) #Clear
    #Dibujamos líneas horizontales
    for y in range(0,DISPLAY_SIZE[1],5):
        display.hline(y,y,60,1)
    #Parámetros de método hline
    #  x : coordenada horizontal de inicio de linea
    #  y : coordenada vertical de inicio de linea
    #  width : anchura de linea. No puede ser negativo
    #  color: color de linea 0=negro, 1=blanco
    display.show()
    time.sleep(1)
    #Dibujamos líneas verticales
    for x in range(0,DISPLAY_SIZE[1],5):
        display.vline(x,x,DISPLAY_SIZE[1]-x,1)
    #Parámetros de método vline
    #  x : coordenada horizontal de inicio de linea
    #  y : coordenada vertical de inicio de linea
    #  height : altura de linea. No puede ser negativo
    #  color: color de linea 0=negro, 1=blanco
    display.show()
    time.sleep(1)
    display.fill(0) #Clear
    for x in range(0,DISPLAY_SIZE[0],10):
        display.line(0,0,x,DISPLAY_SIZE[1]-1,1)
    #Parámetros de método line
    #  coordenada horizontal de inicio de linea
    #  coordenada vertical de inicio de linea
    #  coordenada horizontal de fin de linea
    #  coordenada vertical de fin de linea
    #  color: color de linea 0=negro, 1=blanco
    display.show()
    time.sleep(1)

def test7(): #Test de texto
    display.fill(0) #Clear
    for x in range(0,60,10):
        display.text('Hello World',x,x,1)
    display.show()
    time.sleep(1)

def test8(): #Test de contraste
    loops=500
    contrast=0
    delta=1
    while loops>0:
        display.contrast(contrast)
        time.sleep_ms(10)
        contrast+=delta
        #El contraste puede tener cualquier valor entre 0 y 255
        #Pero a partir de 64 el cambio es imperceptible
        #Por eso sólo variamos entre 0 y 64
        if contrast==0 or contrast==64:
            delta=-delta
        loops-=1
        #Nota. Un cambio de contraste no requiere el método show() para ser aplicado

def test9(): #Power off
    #Desactivamos el display para consumir menos
    #No se pierde la memoria de video del display
    display.poweroff()
    time.sleep(1)
    display.poweron() #Reactivamos el display
    time.sleep(1)

def test10(): #Invert
    display.invert(1) #Invertimos los colores del display
    time.sleep(1)
    #Recuperamos los colores originales del display
    #  0 = negro
    #  1 = blanco
    display.invert(0)
    time.sleep(1)
    
def test11(): #Rotate
    #El método rotate rota la pantalla 180 grados
    #Por defecto la pantalla ya está rotada
    display.rotate(False) #Damos la vuelta
    time.sleep(1)
    display.rotate(True) #La dejamos bien
    time.sleep(1)

def test12(): #Scroll
    #Desplazaremos todo el contenido de la pantalla hacia abajo a la derecha
    #Para que no queden restos, borraremos las dos primeras columnas y
    #la primera fila
    display.hline(0,0,DISPLAY_SIZE[0],0) #Borramos primera fila
    display.fill_rect(0,0,2,DISPLAY_SIZE[1],0) #Borramos primeras dos columnas
    for i in range(64):
        display.scroll(2,1)
        #Nota
        #Los parámetros del método scroll son las coordenadas del vector
        #desplazamiento. Admite valores negativos
        display.show()
    
#Instanciamos clase de gestión de interface de comunicaciones
#Parámetros:
#  scl : objeto Pin de SCL
#  sda : objeto Pin de SDA
#  freq : frecuencia. Este display admite 700KHz
i2c=I2C(scl=Pin(pinSCL),sda=Pin(pinSDA),freq=700000)
#Instanciamos clase de gestor de display
#Parámetros:
#  anchura de display
#  altura de display
#  interface de conexión
display=ssd1306.SSD1306_I2C(DISPLAY_SIZE[0],DISPLAY_SIZE[1],i2c)
test1()
test2()
test3()
test4()
test5()
test6()
test7()
test8()
test9()
test10()
test11()
test12()
