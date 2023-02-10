#Autor: Ramón Junquera
#Fecha: 20221229
#Gestión de timers

# ESP8266 tiene varios timers con distintos indentificadores
# pero son utilizados para por puertos serie, wifi, PWM, etc.
# La mejor solución para no tener interferencias con estos servicios
# es utilizar siempre el identificador -1 que no corresponde a 
# ningún timer hardware. Se crea un un timer por software.
# Además no hay problema en crear varios objetos con timers
# virtuales. Cada uno funcionará de manera independiente.
# El único inconveniente es que no tiene la misma precisión que un
# Timer por hardware

from machine import Timer
import time

#Función de prueba. Admite un parámetro que no se tiene en cuenta
def function1(t):
    print("Función 1")

print("Inicio de programa")
t1=Timer(-1) #Intanciamos un nuevo timer

#Ejemplo de timer ONE_SHOT simple
#Lo inicializamos para que sólo se ejecute una vez, dentro de 3 segundos y
#entonces llame a la función function1
#La función se llamará pasando como parámetro el propio objeto Timer
t1.init(period=3000,mode=Timer.ONE_SHOT,callback=function1)
#Contamos hasta 5 mostrando un número por segundo
for i in range(1,6):
    print(i)
    time.sleep(1)
#Mostrará los número del 1 al 3, se llamará a la función y continuará con el
#4 y el 5.

#Ejemplo de gestión de 2 timers
t2=Timer(-1) #Intanciamos un nuevo timer
#Como en el ejemplo anterior definimos el primer Timer para que se ejecute una vez
#dentro de 3 segundos y llame a la función function1
t1.init(period=3000,mode=Timer.ONE_SHOT,callback=function1)
#Inicializamos un segundo Timer, parecido al anterior. Sólo se ejecuta una vez
#dentrod e 4 segundos, pero llamará a una función lambda definida en el parámetro
t2.init(period=4000,mode=Timer.ONE_SHOT,callback=lambda t:print("Función lambda"))
for i in range(1,6):
    print(i)
    time.sleep(1)
#Mostrará los número del 1 al 3, se llamará a function1, se mostrará el 4, se llamará
#a la función lambda y finalmente se mostrará el 5

#Ejemplo de timer periódico simple y como pararlo
t1.init(period=1000,mode=Timer.PERIODIC,callback=function1)
time.sleep(3)
t1.deinit() #Desactivamos el Timer
print("Timer desactivado")
time.sleep(3)

#Ejemplo de 2 timers periódicos
t1.init(period=1000,mode=Timer.PERIODIC,callback=function1)
t2.init(period=1500,mode=Timer.PERIODIC,callback=lambda t:print("Timer periodico"))
#Una vez que finaliza el programa, los timers periódicos activos siguen funcionando!

print("Fin de programa")

