#Autor: Ramón Junquera
#Fecha: 20221226

#Por defecto MicroPython tiene activado la interface de wifi punto de acceso
#Es conveniente dejar activas sólo las interfaces que vayamos a utilizar
#En este ejemplo mostramos el estado de las interfaces wifi inicialmente, las desactivamos,
#y activamos la de punto de acceso local con nombre y contraseña personalizados.
#Por defecto, la dirección del dispositivo es 192.168.4.1
#Nota. En la documentación de MicroPython están mal nombrados los parámetros de configuración.

import network
from time import sleep

#Interfaces wifi
WIFI_CLI = network.WLAN(network.STA_IF) #Modo cliente
WIFI_AP = network.WLAN(network.AP_IF) #Modo punto de acceso
#Propiedades del punto de acceso externo
WIFI_AP_SSID="ESP8266"
WIFI_AP_PASSWORD="12345678"

#Muestra el estado de las interfaces wifi
def wifi_status():
    print("Interface wifi cliente: " + ("ON" if WIFI_CLI.active() else "OFF"))
    print("Interface wifi punto de acceso: " + ("ON" if WIFI_AP.active() else "OFF"))

def wifi_ap_connect():
    WIFI_AP.active(True)
    print("Activando punto de acceso local")
    print("SSID actual:",WIFI_AP.config('essid'))
    WIFI_AP.config(essid=WIFI_AP_SSID,password=WIFI_AP_PASSWORD)
    print("nuevo SSID:",WIFI_AP.config('essid'))

wifi_status()
#Desactivamos las interfaces wifi
WIFI_CLI.active(False)
WIFI_AP.active(False)
print("Interfaces wifi desactivadas")
wifi_status()
wifi_ap_connect()
wifi_status()
