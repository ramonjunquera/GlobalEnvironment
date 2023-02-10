#Autor: Ramón Junquera
#Fecha: 20221229
#Ejemplo de efecto Kitt

import machine # Gestión de GPIO
import time # Gestión de tiempos
# Seleccionamos 8 pines para los leds
GPIOs=(16,5,4,0,2,14,12,13)
# Creamos una lista de los pines de los GPIOs
# Todos configurados como salida
pines=list(map(lambda x:machine.Pin(x, machine.Pin.OUT),GPIOs))
index=0 # pin procesado
delta=1 # sentido
while True: # Bucle infinito
 pines[index].off() # Apagamos el led que está encendido
 index+=delta # Calculamos el siguiente pin
 if index==0 or index==7: # Si hemos llegado a un extremo...
  delta=-delta # ...cambiamos el sentido
 pines[index].on() # Encendemos el siguiente pin
 time.sleep_ms(100) # Esperamos un momento
