#Autor: Ramón Junquera
#Fecha: 20221229
#Entrada analógica. Conversión ADC. Datos aplicados a led integrado

#En este ejemplo, en vez de enviar el valor analógico leido
#por el puerto serie lo convertiremos en intensidad del led 
#integrado en placa

#Puesto que el GPIO 16 no puede ser utilizado para PWM y es uno de los
#dos que tienen asociados a un led integrado, utilizaremos en
#este ejemplo el otro que es el que está asociado al GPIO 2

#Puesto que el rango de intensidad PWM es [0,1023]
#y el de entrada analógico [0,1024]
#Sólo nos tendremos que preocupar de reducir a 1023 el pwm
#cuando la entrada sea 1024

#Puesto que los led integrados están intercambiados con el
#voltaje, hemos intercambiado los cables del potenciómetro
#respecto al ejemplo anterior

#Nota.
#El led integrado nunca se termina de apagar, porque la resistencia variable
#nunca corta el circuito. Con el máximo valor de la resistencia aun tenemos
#lecturas de 30 aproximadamente.

import machine # Gestión de GPIO
# Creamos el objeto de gestión de la entrada analógica
adc=machine.ADC(0)
# Creamos el objeto de gestión de pwm del GPIO 2 (led integrado)
pwm=machine.PWM(machine.Pin(2,machine.Pin.OUT),freq=1000)
while True: #Bucle infinito
 analog_value=adc.read() #Leemos el valor analógico
 #Eliminamos la posibilidad de 1024
 analog_value=analog_value if analog_value<1024 else 1023
 pwm.duty(analog_value) #Aplicamos valor al PWM
