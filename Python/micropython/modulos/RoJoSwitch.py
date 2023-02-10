#Autor: Ramón Junquera
#Fecha: 20221229

#Módulo RoJoSwitch
#Gestión de un interruptor
#Utiliza las resistencias PULL_UP internas
#El interruptor debe ir conectado a tierra y al pin que lee su estado

from machine import Pin,Signal
import time

class RoJoSwitch:
    def _check(self): #Comprueba el estado del pulsador
        now=time.ticks_ms() #Tomamos nota de la hora actual
        if self._pin.value()!=self._lastStatus: #Si ha cambiado el estado del pulsador...
            self._lastStatus=not self._lastStatus
            if now-self._lastTime>self.delayTime: #Si podemos aceptar una nueva pulsación...
                if self._lastStatus: #Si se acaba de soltar...
                    self._buttonReleased=True;
                else: #Si se acaba de pulsar...
                    self._buttonPressed=True
            _lastTime=now
    def __init__(self,pin):
        self._pin=Pin(pin,Pin.IN,Pin.PULL_UP)
        self._lastTime=0 #Momento del último cambio...ahora
        self._lastStatus=False #Último estado comprobado
        self._buttonPressed=False #Flag de si ha sido pulsado
        self._buttonReleased=False #Flag de si ha sido soltado
        self.delayTime=50 #Tiempo de espera en ms para evitar efecto rebote
        self._check()
        self._buttonPressed=self._buttonReleased=False #Inicialmente sin actividad
    def pressed(self): #Se ha pulsado?
        self._check()
        if self._buttonPressed: #Si se ha pulsado...
            self._buttonPressed=False #...Limpiamos el flag
            return True
        return False
    def released(self): #Se ha soltado?
        self._check()
        if self._buttonReleased: #Si se ha soltado...
            self._buttonReleased=False #Limpiamos el flag
            return True
        return False
    def pressing(self): #Se está pulsando?
        self._check()
        return not self._lastStatus
    def pin(self): #Devuelve el objeto Pin creado para el pulsador
        return self._pin

