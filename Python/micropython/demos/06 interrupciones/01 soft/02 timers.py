#Autor: Ramón Junquera
#Fecha: 20221231
#El pulsador enciende el led integrado durante 2 segundos

#Módulos necesarios: RoJoSwitch.py

#Utilizamos el módulo RoJoSwitch para simplificar la gestión del interruptor integrado
#Se configura el pin del led integrado como salida y con estado HIGH/apagado inicial.
#Se invierte la señal para que coincida con la realidad: estado=HIGH -> led=ON
#Cuando se detecte una pulsación se encenderá el led y se lanzará un timer de una sola
#ejecución para que a los dos segundos se encargue de apagar el led mediante una
#función lambda
#Si pulsamos el botón cuando el led aun está encendido, se reaprovechará el timer,
#reconfigurándolo para que se ejecute tras dos segundos del momento actual.
#Así que el led permanecerá encendido 2 segundos más
#En este ejemplo utilizamos timers virtuales (por software).
#Esto ocurre cuando definimos el identificador del timer como -1.
#ESP8266 tiene dos timers por hardware (0 y 1) pero son utilizados para comunicaciones
#así que no quedan disponibles para el usuario.
#En cambio ESP32, ofrece sus 4 timers por hardware (0,1,2,3) para programarlos.

DEVICE_ID="ESP-12E"
#DEVICE_ID="LOLIN32"

from RoJoSwitch import RoJoSwitch
import machine

if DEVICE_ID=="ESP-12E":
    pinSwitch=0 #D3
    pinLed=16 #D0
elif DEVICE_ID=="LOLIN32":
    pinSwitch=22
    pinLed=5

switch=RoJoSwitch(pinSwitch) #Instanciamos un interruptor
led=machine.Signal(machine.Pin(pinLed,machine.Pin.OUT,value=1),invert=True) #Configuramos led integrado con estado invertido
tim=machine.Timer(-1) #Timer virtual
while True: #Bucle infinito
    if switch.pressed(): # Si el interruptor se ha pulsado
        led.on() #Encendemos el led
        #Configuramos el timer para que dentro de 2 segundos apague el led
        tim.init(period=2000,mode=machine.Timer.ONE_SHOT,callback=(lambda t:led.off()))


