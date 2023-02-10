#Autor: Ramón Junquera
#Fecha: 20221225

#Por defecto MicroPython tiene activado la interface de wifi punto de acceso
#Es conveniente dejar activas sólo las interfaces que vayamos a utilizar
#En este ejemplo mostramos el estado de las interfaces wifi inicialmente, las desactivamos,
#activamos la de conexión como cliente a un punto de acceso externo y nos conectamos

import network
from time import sleep

#Interfaces wifi
WIFI_CLI = network.WLAN(network.STA_IF) #Modo cliente
WIFI_AP = network.WLAN(network.AP_IF) #Modo punto de acceso
#Propiedades del punto de acceso externo
WIFI_AP_SSID="my_ssid"
WIFI_AP_PASSWORD="my_password"

#Muestra el estado de las interfaces wifi
def wifi_status():
    print("Interface wifi cliente: " + ("ON" if WIFI_CLI.active() else "OFF"))
    print("Interface wifi punto de acceso: " + ("ON" if WIFI_AP.active() else "OFF"))

#Conectamos como cliente a un punto de acceso
def wifi_cli_connect():    
    WIFI_CLI.active(True)
    print('Conectando como wifi cliente',end="")
    WIFI_CLI.connect(WIFI_AP_SSID,WIFI_AP_PASSWORD)
    while not WIFI_CLI.isconnected():
        print(".",end="")
        sleep(0.5)
    print('ok\nnetwork config:',WIFI_CLI.ifconfig())

wifi_status()
#Desactivamos las interfaces wifi
WIFI_CLI.active(False)
WIFI_AP.active(False)
print("Interfaces wifi desactivadas")
wifi_status()
wifi_cli_connect()
wifi_status()
