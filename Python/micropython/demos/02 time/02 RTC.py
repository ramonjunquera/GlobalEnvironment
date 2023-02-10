#Autor: Ramón Junquera
#Fecha: 20221227

#RTC
#Comenzamos desactivando la interface de punto de acceso local para que no interfiera en el
#rendimiento del sistema.
#Activamos la interface de conexión cliente a un punto de acceso
#Es importante que el punto de acceso nos de conexión a Internet, para poder alcanzar un
#servidor NTP con el que podamos sincronizar la hora.
#Si no sincronizamos el reloj interno, cada vez que arranque lo hará desde el 01/01/2000.
#En en IDE Thonny se pude configurar que el reloj interno para que tome su hora de:
#- El PC que con el que tiene conexión (Sincronizar el rejol de tiempo real del dispositivo)
#- El servidor NTP que tenga definido el PC (Utilizar la hora local en el reloj de tiempo real)
#Desde el menú Ejecutar/Configurar intérprete...
#En cualquiera de los dos casos, cada vez que se reinicie el dispositivo se sincronizará el RTC
#siendo innecesario todo el programa del ejemplo

import network
import ntptime
from machine import RTC
from time import sleep
import machine

#Credenciales del punto de acceso
WIFI_AP_SSID="my_ssid"
WIFI_AP_PASSWORD="my_password"

#Conectamos como cliente a un punto de acceso
def wifi_cli_connect():
    WIFI_CLI=network.WLAN(network.STA_IF) #Modo cliente
    WIFI_CLI.active(True)
    print('Conectando como wifi cliente',end="")
    WIFI_CLI.connect(WIFI_AP_SSID,WIFI_AP_PASSWORD)
    while not WIFI_CLI.isconnected():
        print(".",end="")
        sleep(0.5)
    print('ok\nnetwork config:',WIFI_CLI.ifconfig())
    
#Sincroniza el reloj interno (RTC) con un servidor NTP en internet
def sync_clock():
    #Podemos fijar el servidor NTP. POr defecto utiliza pool.ntp.org
    ntptime.host="1.europe.pool.ntp.org"
    ntptime.settime() #Sincronizamos reloj interno con servidor NTP en Internet
    (year,month,day,weekday,hour,minute,second,milisecond)=RTC().datetime() #Obtenemos fecha actual
    RTC().datetime((year,month,day,weekday,hour+1,minute,second,milisecond)) #Corregimos a GMT+1

#Muestra la fecha y hora actuales
def show_date():
    d=RTC().datetime() #Decorador para reducir texto en parámetros
    print("{:02d}/{:02d}/{} {:02d}:{:02d}:{:02d}".format(d[2],d[1],d[0],d[4],d[5],d[6])) #dd/mm/yyyy hh:mm:ss

network.WLAN(network.AP_IF).active(False) #Desactivamos interface wifi de punto de acceso local
wifi_cli_connect() #Conectamos wifi a punto de acceso
sync_clock() #Sincronizamos hora con Internet
show_date()
