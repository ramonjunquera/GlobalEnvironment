#Autor: Ramón Junquera
#Fecha: 20221229
#PWM. Efecto Kitt

#En este ejemplo haremos el efecto Kitt con PWM
#Utilizamos 8 leds, pero podríamos usar un número distinto

#Los pines los definimos como constantes en una tupla inicial
#Creamos una lista con los objetos PWM para poder gestionar la intensidad

#Consideraciones
#Aunque la máxima frecuencia de PWM es de 1000, no la utilizaremos
#Micropython utiliza IRQs para generar PWM
#Con la máxima frecuencia sobrecargaremos la CPU de IRQs para generar los PWM
#Lo dejaremos en 100Hz que es suficiente como para que no parpadee a la vista

#Aun con 100Hz, nos dedicamos a leer y escribir la intensidad de todos
#los objetos PWM 10 veces por segundo. Esto sigue siendo demasiada sobrecarga
#de CPU. Es esta situación lo que ocurre es que el programa nunca da error
#pero hay momentos en que las luces dejan de moverse, porque no le da tiempo
#a la CPU a hacer todos los cambios
#Si bajásemos los cambios a 1 por segundo, este efecto indeseado ocurriría
#con menos frecuencia.
#Para solventarlo, cada vez que solicitamos un cambio, llamamos al comando
#machine.idle() que se encarga de ejecutar todos los procesos de fondo
#que haya pendientes hasta la siguiente IRQ. Esta línea evita el efecto
#de parada.

#También ayuda a que este efecto no ocurra que forcemos la 
#velocidad de proceso de la CPU a 160 MHz

#Son necesarias las tres acciones (frecuencia PWM a 100Hz, idle y
#CPU a 160MHz) para que funcione correctamente.

import machine # Gestión de GPIO
import time # Gestión de tiempos

machine.freq(160000000) # Forzamos la CPU a 160MHz
pins=(5,4,0,2,14,12,13,15)
# Creamos una lista que contendrá los objetos PWM de cada GPIO
pwm=list(map(lambda x:machine.PWM(machine.Pin(x,machine.Pin.OUT),duty=0),pins))
pwm[0].freq(100) #Fijamos frecuencia a 100Hz
current_led=0 #led procesado
delta=1 #sentido

while True: #Bucle infinito
 #Anotamos que el led procesado debe tener la máxima intensidad
 pwm[current_led].duty(1023)
 #Calculamos el siguiente led
 current_led+=delta
 # Si hemos llegado a un extremo cambiamos el sentido
 if current_led==0 or current_led==len(pins)-1: delta=-delta
 # Esperamos un momento
 time.sleep_ms(100)
 # Recorremos todos los leds
 for i in range(len(pins)):
  d=pwm[i].duty() #Anotamos su intensidad
  d=d-200 if d>200 else 0 # Le quitamos 200 sin dejar que quede en negativo
  pwm[i].duty(d) # Aplicamos la nueva intensidad
  d=machine.idle() # Dejamos que se ejecuten los procesos de fondo
