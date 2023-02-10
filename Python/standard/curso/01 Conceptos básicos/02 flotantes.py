#Autor: Ramón Junquera
#Fecha: 20221219

# Algunas operaciones producen números flotantes (de coma flotante)
print(3/4) #Resultado: 0.75
print(1/3) #Resultado: 0.3333333
#Los ceros innecesarios siempre se eliminan
print(9.87650000) #Resultado: 9.8765
print(-0032.5) #Resultado: -32.5
# Los números flotantes siempre tienen al menos un decimal
# Aunque sea cero
print(8/4) #Resultado: 2.0
print(2*3.0) #Resultado: 6.0
print(4+1.65) #Resultado:5.65
print(1+2+3.0+4) #Resultado: 10.0

# La función round() permite redondear un valor a un número
# determinado de decimales
# Ejemplo. Redondeamos 3.4567 a 2 decimales
print(round(3.4567,2)) #Resultado: 3.46