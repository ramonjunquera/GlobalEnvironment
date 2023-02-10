#Autor: Ramón Junquera
#Fecha: 20221231
#Módulo RoJoSSD1331

import machine
import time #Para sleep

class DisplayRange: #Estructura para almacenar rangos
    def __init__(self): #Constructor
        self.visible=False
        p=(0,0)
        size=(0,0)
        
class RoJoSSD1331:
    def _write_command(self,t): # Envía los valores (bytes) de una lista como comandos
        self._pinDC.off() # Se enviará un comando
        self._pinCS.off() # Activamos envío
        # Creamos un buffer de un sólo byte
        # Sería más cómodo crearlo del tamaño de la lista, pero consume más memoria
        buf=bytearray(1)
        for i in t: # Recorremos todos los elementos de la lista
            buf[0]=i # Lo guardamos el buffer
            self._spi.write(buf) # Lo enviamos por SPI
        self._pinCS.on() # Desactivamos envío
    def clear(self): # Pinta la pantalla de negro con un bloque
        self.block((0,0),self.size,(0,0,0))
    def sleep(self,mode): #Hibernación
        #En hibernación se desactiva del display, pero permite seguir dibujando
        #Cuando se salga de hibernación y se vuelva a activar se mostrará el resultado
        if mode: #Entrar en hibernación...
            self._write_command([0xAE #Display Off
            ,0xB0,0x1A]) #Enable Power Save mode
        else: #Salir de hibernación...
            self._write_command([0xB0,0x0B #Disable Power Save Mode
            ,0xAF]) #Display On
    def reset(self): # Resetea el display con el pin RST
        self._pinRST.off()
        time.sleep_ms(10)
        self._pinRST.on()
        time.sleep_ms(10)
        # Enviamos la secuencia de inicialización
        self._write_command([0xA0,0x72 # Fijamos orden de componentes de color (SSD1331_CMD_SETREMAP) a RGB
        ,0xA1,0x00 # Fijamos como primera línea (SSD1331_CMD_STARTLINE) la 0
        ,0xA2,0x00 # SSD1331_CMD_DISPLAYOFFSET
        ,0xA4 # SSD1331_CMD_NORMALDISPLAY
        ,0xA8,0x3F # SSD1331_CMD_SETMULTIPLEX 1/64 duty
        ,0xAD,0x8E # SSD1331_CMD_SETMASTER
        ,0xB0,0x0B # SSD1331_CMD_POWERMODE
        ,0xB1,0x31 # SSD1331_CMD_PRECHARGE
        ,0xB3,0xF0 # SSD1331_CMD_CLOCKDIV 7:4 = Oscillator Frequency, 3:0 = CLK Div Ratio (A[3:0]+1 = 1..16)
        ,0x8A,0x64 # SSD1331_CMD_PRECHARGEA
        ,0x8B,0x78 # SSD1331_CMD_PRECHARGEB
        ,0x8C,0x64 # SSD1331_CMD_PRECHARGEA
        ,0xBB,0x3A # SSD1331_CMD_PRECHARGELEVEL
        ,0xBE,0x3E # SSD1331_CMD_VCOMH
        ,0x87,0x06 # SSD1331_CMD_MASTERCURRENT
        ,0x81,0x91 # SSD1331_CMD_CONTRASTA
        ,0x82,0x50 # SSD1331_CMD_CONTRASTB
        ,0x83,0x7D # SSD1331_CMD_CONTRASTC
        ,0xAF]) # Activamos el display (SSD1331_CMD_DISPLAYON)
        self.clear() # Borramos la pantalla
        self.sleep(False) #Salimos del modo de bajo consumo
    def _fill(self,f): # Activa/desactiva el relleno de los rectángulos
        self._write_command([0x26,1 if f==True else 0])
    #Dibuja un rectángulo. Función interna
    #  p : tupla/lista con coordenadas x,y de esquina superior izquierda
    #  size : tupla/lista con anchura,altura
    #  borderColor : tupla/lista RGB con color de borde
    #  fillColor : tupla/lista RGB con color de relleno
    def _rect(self,p,size,borderColor,fillColor):
        self._write_command([0x22,p[0],p[1],p[0]+size[0]-1,p[1]+size[1]-1,borderColor[0],borderColor[1],borderColor[2],fillColor[0],fillColor[1],fillColor[2]])
        time.sleep_ms(3) # Es necesario darle un tiempo para que termine de dibujar
    #Constructor
    #  gpioRST : pin de reset
    #  gpioDC : pin Data/Command
    #  gpioCS : pin ChipSet
    #  spiID : id de SPI. 1 para ESP8266, 2 para ESP32
    def __init__(self,gpioRST,gpioDC,gpioCS,spiID):
        self.size=(96,64) #Tamaño del display (anchura,altura)
        # Guardamos los parámetros como objetos PIN
        # Todos son pines de salida, inicialmente a HIGH
        self._pinRST=machine.Pin(gpioRST,machine.Pin.OUT,value=1)
        self._pinDC=machine.Pin(gpioDC,machine.Pin.OUT,value=1)
        self._pinCS=machine.Pin(gpioCS,machine.Pin.OUT,value=1)
        # Creamos el objeto SPI privado
        # Este display admite como máxima velocidad 20MHz
        self._spi=machine.SPI(spiID,baudrate=20000000,polarity=0,phase=0)
        self.reset() # Reseteamos el display
    #Dibuja un rectángulo relleno
    # p : tupla/lista con coordenadas x,y de esquina superior izquierda
    # size : tupla/lista con anchura,altura
    # borderColor : tupla/lista RGB con color de borde
    # fillColor : tupla/lista RGB con color de relleno
    def rectFill(self,p,size,borderColor,fillColor):
        self._fill(True) # Nos aseguramos que los rectángulos se rellenen
        self._rect(p,size,borderColor,fillColor) # Llamamos a la rutina interna de dibujo
    #Dibuja un rectángulo sin relleno
    # p : tupla/lista con coordenadas x,y de esquina superior izquierda
    # size : tupla/lista con anchura,altura
    # colorBorder : tupla/lista RGB con color de borde
    def rect(self,p,size,borderColor):
        self._fill(False) # Nos aseguramos que los rectángulos no se rellenen
        self._rect(p,size,borderColor,(0,0,0)) # Llamamos a la rutina interna de dibujo
    #Dibuja un bloque. Función interna. Sin comprobaciones
    # p : tupla/lista con coordenadas x,y de esquina superior izquierda
    # size : tupla/lista con anchura,altura
    # color : tupla/lista RGB con color
    def _block(self,p,size,color):
        self.rectFill(p,size,color,color)
    #Dibuja un bloque
    # p : tupla/lista con coordenadas x,y de esquina superior izquierda
    # size : tupla/lista con anchura,altura
    # color : tupla/lista RGB con color
    #Devuelve True si tiene parte visible
    def block(self,p,size,color):
        r=self.visibleRange(p,size,self.size) #Rango visible
        if not r.visible:
            return False #No tiene parte visible
        #El bloque tiene parte visible
        self._block(r.p,r.size,color)
        return True
    #Dibuja un pixel. Función interna. Sin comprobaciones
    # p : tupla/lista con coordenadas x,y del punto
    # color : tupla/lista RGB con color
    def _drawPixel(self,p,color):
        self.rect(p,(1,1),color)
    #Dibuja un pixel
    # p : tupla/lista con coordenadas x,y del punto
    # color : tupla/lista RGB con color
    #Devuelve True si el pixel es visible
    def drawPixel(self,p,color):
        #Si las coordenadas están fuera de rango...terminamos
        if p[0]<0 or p[0]>=self.size[0] or p[1]<0 or p[1]>=self.size[1]:
            return False #Pixel no visible
        self._drawPixel(p,color) #Dibujamos el pixel con la función interna
        return True #Pixel visible
    #Dibuja una linea
    # p1 : tupla/lista con coordenadas x,y del primer punto
    # p2 : tupla/lista con coordenadas x,y del segundo punto
    # color : tupla/lista RGB con color
    def line(self,p1,p2,color):
        self._write_command([0x21,p1[0],p1[1],p2[0],p2[1],color[0],color[1],color[2]])
        time.sleep_ms(1) # Es necesario darle un tiempo para que termine de dibujar
    #Oscurece un árez rectangular
    # p : tupla/lista con coordenadas x,y de esquina superior izquierda
    # size : tupla/lista con anchura,altura
    def darker(self,p,size):
        self._write_command([0x24,p[0],p[1],p[0]+size[0]-1,p[1]+size[1]-1])
        time.sleep_ms(2) # Es necesario darle un tiempo para que termine de dibujar
    #Copia un área en otra
    # p1 : tupla/lista con coordenadas x,y de esquina superior izquierda de área origen
    # size: tupla/lista con anchura,altura de área origen
    # p2 : tupla/lista con coordenadas x,y de esquina superior izquierda de área destino
    def copy(self,p1,size,p2):
        self._write_command([0x23,p1[0],p1[1],p1[0]+size[0]-1,p1[1]+size[1]-1,p2[0],p2[1]])
        time.sleep_ms(3) # Es necesario darle un tiempo para que termine de dibujar
    #Dibuja una triángulo sin relleno
    # p1 : tupla/lista con coordenadas x,y del primer punto
    # p2 : tupla/lista con coordenadas x,y del segundo punto
    # p3 : tupla/lista con coordenadas x,y del tercer punto
    # borderColor : tupla/lista RGB con color del borde
    def triangle(self,p1,p2,p3,borderColor):
        self.line(p1,p2,borderColor)
        self.line(p1,p3,borderColor)
        self.line(p2,p3,borderColor)
    #Calcula el rango visible
    # p : tupla/lista con coordenadas x,y de esquina superior izquierda
    # size : tupla/lista con anchura,altura
    # displaySize : tupla/lista con anchura,altura del display
    #Devuelve un objeto DisplayRange con el resultado
    def visibleRange(self,p,size,displaySize):
        if size[0]<0: #Si la anchura es negativa...
            p=(p[0]+size[0],p[1])
            size=(-size[0],size[1])
        if size[1]<0: #Si la altura es negativa...
            p=(p[0],p[1]+size[1])
            size=(size[0],-size[1])
        res=DisplayRange #Instanciamos rango
        if size[0]!=0 and size[1]!=0 and displaySize[0]!=0 and displaySize[1]!=0: #Si tiene dimensión...
            #Si parte del bloque está dentro de pantalla...
            if p[0]<displaySize[0] and p[1]<displaySize[1] and p[0]+size[0]>0 and p[1]+size[1]>0:
                res.visible=True
                #Calculamos la zona que ocupará
                res.p=(0 if p[0]<0 else p[0],0 if p[1]<0 else p[1])
                res.size=(size[0]-res.p[0]+p[0] if p[0]+size[0]-1<displaySize[0] else displaySize[0]-p[0]-1,size[1]-res.p[1]+p[1] if p[1]+size[1]-1<displaySize[1] else displaySize[1]-p[1]-1)
        return res
