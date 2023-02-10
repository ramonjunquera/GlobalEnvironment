#Autor: Ramón Junquera
#Fecha: 20221219

# Bucles while
# Sintaxis: while +  condición + :
# Mientras se cumplala condición, se ejecutarán todas las líneas
# identadas que haya a continuación

# Ejemplo que muestra los 5 primeros números en pantalla
i=1
while i<=5:
 print(i)
 i+=1
# Ejemplo que muestra los números del 3 al 0
i=3
while i>=0:
 print(i)
 i-=1
# Ejemplo de comando break para salir de un bucle
# Muestra los 100 primeros números
i=1
while True:
 print(i)
 if i==100:
  break
 i+=1
# Ejemplo de comando continue. Vuelve al inicio del bucle
# Escribe los números del 1 al 10 saltándose el 3
i=0
while i<10:
  i+=1
  if i==3:
    continue
  print(i)
# Ejemplo. Escribe los números del 1 al 10 saltándose el 4
i=0
while True:
 i+=1
 if i>10:
  break
 if i==4:
  continue
 print(i)
# Bucle infinito
i=1
while True:
 print(i)
 i+=1