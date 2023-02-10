#Autor: Ramón Junquera
#Fecha: 20221229
#Entrada analógica. Conversión ADC. Salida de datos por puerto serie

#Las placas ESP8266 sólo tienen un pin de entrada analógica
#llamado A0. Es el único conectado a un conversor analógico
#digital (ADC).

#Para crear el objeto que permite leerlo, habitualmente utilizaremos:
#adc=machine.ADC(0)
#Siempre es 0 porque sólo tenemos un pin de entrada analógica

#Para leer su valor utilizaremos
#adc.read() que nos devolverá un valor en el rango [0,1024]
#1024 es el valor que representa cualquier voltaje superior
#al permitido

#Como el resto de pines del microprocesador, soporta un voltaje
#máximo de 3.3V

import machine #Gestión de GPIO
import time #Gestión de tiempos
# Creamos el objeto de gestión de la entrada analógica
adc=machine.ADC(0) 
while True: #Bucle infinito
 print(adc.read()) #Mostramos el valor leido
 time.sleep_ms(200) #Cada 2 décimas de segundo

