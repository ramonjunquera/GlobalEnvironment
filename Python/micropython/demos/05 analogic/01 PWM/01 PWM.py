#Autor: Ramón Junquera
#Fecha: 20221231
#PWM
#Cambiamos la intensidad de un led integrado

#Se puede aplicar PWM a cualquier GPIO excepto al 16
#El rango de intensidad tiene una precisión de 10 bits [0,1023]
#Se puede fijar la frecuencia que es única y afecta a todos
#los pines. La frecuencia está en el rango [1,1000] y se mide en Hz

#Las instrucciones más comunes son:
#  pwm0=PWM(Pin(0)) #Crea un objeto PWM de un pin
#  pwm0.freq() #Lee la frecuencia actual
#  pwm0.freq(1000) #Fija la frecuencia a 1000 (máximo)
#  pwm0.duty() #Lee la intensidad actual de un pin
#  pwm0.duty(200) #Fija la intensidad de un pin
#  pwm0.deinit() #Desactiva PWM de un pin

#Ejemplo de creación de un objeto PWM pasando todos los parámetros
#  pwm2=PWM(Pin(2),freq=500,duty=512)

#En ESP-12E que el GPIO 16 no puede ser utilizado para PWM y es uno de los
#dos que tiene asociados a un led integrado, utilizaremos en este
#ejemplo el otro que es el que está asociado al GPIO 2

#El objeto PWM pertenece al módulo machine

#DEVICE_ID="ESP-12E"
DEVICE_ID="LOLIN32"

import machine #Gestión de GPIO
import time #Gestión de tiempos

if DEVICE_ID=="ESP-12E":
    pinLed=2
elif DEVICE_ID=="LOLIN32":
    pinLed=5

led=machine.PWM(machine.Pin(pinLed,machine.Pin.OUT),freq=1000,duty=0)
#Podemos dividir en dos pasos la creación del pin PWM
#led_digital=machine.Pin(pinLed,machine.Pin.OUT) #Pin digital de salida para el led
#led=machine.PWM(led_digital,freq=1000,duty=0) #pin PWM

level=0 #Intensidad de led
delta=1 #Sentido
while True: #Bucle infinito
     led.duty(level) #Aplicamos intensidad
     level+=delta #Calculamos la siguiente intensidad
     if level==0 or level==1023: #Si hemos llegado a un extremo...
          delta=-delta #Cambiamos de sentido
     time.sleep_ms(1) #Esperamos un milisegundo



