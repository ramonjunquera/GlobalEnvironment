#Autor: Ramón Junquera
#Fecha: 20221226

#Ejemplo de cómo tomar el número de milisegundos transcurridos desde el último reinicio,
#diferencias de tiempos, esperas en segundos, milisegundos y microsegundos.
#La razón por la que la mayoría de las veces se tarda 1ms más de lo previsto es que no
#sólo coronometramos el tiempo, sino que lo dedicamos a llamar a funciones y escribir texto.

import time

last_ms=time.ticks_ms()

def check_time():
    global now,last_ms #Utilizaremos variables globales
    now=time.ticks_ms() #Anotamos los milisegundos transcurridos actuales
    print(now-last_ms,"ms") #Calculamos diferencia de tiempo transcurrido
    last_ms=now #Para la próxima llamada
    
    #Nota.
    #Aunque la diferencia de tiempos en milisegundos se puede calcular como una simple
    #resta, es conveniente utilizar el método time.ticks_diff(now,last_ms
    
print("Punto 1. inicio")
time.sleep(1) #Esperamos un segundo
print("Punto 2. ",end="")
check_time()
time.sleep(0.5) #Esperamos medio segundo
print("Punto 3. ",end="")
check_time()
time.sleep_ms(100) #Esperamos 100 milisegundos = 1 décima de segundo
print("Punto 4. ",end="")
check_time()
time.sleep_us(1000) #Esperamos 1000 microsegundos = 1 milisegundo
print("Punto 5. ",end="")
check_time()

                      
