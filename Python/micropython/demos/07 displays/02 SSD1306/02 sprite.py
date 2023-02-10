#Autor: Ramón Junquera
#Fecha: 20230101
#Demostración de uso sprites (FrameBuffer) con módulo ssd1306

#Este módulo ya viene incluido dentro de MicroPython para ESP8266
#No existe en la versión para ESP32
#Internamente tiene una memoria de video que gestiona el driver
#Siempre es necesario pasar esa memoria al display con el método show()

#Suponemos que el identificador I2C del display es el estandar: 0x3C

#El objeto FrameBuffer permite crear sprites en memoria que después se
#pueden enviar al display

#Se pueden visualizar todos los métodos de FrameBuffer con el comando
#  help(framebuf.FrameBuffer)

DEVICE_ID="ESP-12E"

DISPLAY_SIZE=(128,64) #Tamaño del display

from machine import Pin,I2C
import ssd1306
import time #Para sleep
import framebuf

if DEVICE_ID=="ESP-12E":
    pinSDA=4 #D2
    pinSCL=5 #D1

def test1(): #FrameBuffer
    display.fill(0) #Clear
    #Creamos un sprite monocromo de 8x8
    #Parámetros de FrameBuffer
    #  buffer : contiene los datos y se debe dimensionar correctamente
    #  width : anchura del sprite
    #  height : altura del sprite
    #  format: formato
    #    framebuf.MONO_VLSB : monocromo & bits en vertical
    #    framebuf.MONO_HLSB : monocromo & bits en horizontal & bit 7 a la izquierda
    #    framebuf.MONO_HMSB : monocromo & bits en horizontal & bit 0 a la izquierda
    #    framebuf.RGB565 : color 16 bits
    #    framebuf.GS2_HMSB : grises 2 bits
    #    framebuf.GS4_HMSB : grises 4 bits
    #    framebuf.GS8 : grises 8 bits
    sprite=framebuf.FrameBuffer(bytearray(8),8,8,framebuf.MONO_VLSB)
    sprite.line(0,0,7,7,1) #Linea diagonal desde (0,0) a (7,7) en color 1
    sprite.rect(1,1,6,6,1) #Cuadrado
    display.blit(sprite,10,10) #Dibujamops el sprite en la coordenadas (10,10)
    display.show()
    time.sleep(1)
    
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
