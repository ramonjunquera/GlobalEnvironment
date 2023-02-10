#Autor: Ramón Junquera
#Fecha: 20221221

# itertools
# Este módulo incluye ciertas facilidades para trabajar
# con objetos iterables.

# count es un generador similar al funcionamiento de range
# Si no indicamos parámetros comenzará desde cero hasta infinito
# Ejemplo que muestra los números desde el 0 al 10
import itertools
for i in itertools.count(): #Desde 0 hasta infinito
 print(i)
 if i>9: # Ponemos un límite
  break

# Si incluimos un parámetro a la función count() se tomará como
# valor inicial de la secuencia
# Ejemplo que muestra los números desde el 5 al 10
import itertools
for i in itertools.count(5):
 print(i)
 if i>9: # Ponemos un límite
  break

# Si incluimos un segundo parámetro se tomará como el incremento
# entre valores
# Ejemplo que muestra los números pares desde el 0 al 10
import itertools
for i in itertools.count(0,2):
 print(i)
 if i>9: # Ponemos un límite
  break

# cycle es un generador que se aplica a un objeto iterable y 
# devuelve sus elementos cíclicamente. Cuando llega al final
# vuelve a comenzar de nuevo
import itertools
miLista=["uno","dos","tres"]
contador=0 # Inicializamos el contador de elementos a mostrar
for i in itertools.cycle(miLista):
 print(i)
 contador+=1
 if contador==10: # Sólo mostramos 10 elementos
  break

# repeat es un generador que repite el mismo valor (u objeto)
# Si sólo entregamos un parámetro será el objeto a repetir
# y se hará infinitamente
import itertools
contador=0 # Inicializamos el contador de elementos a mostrar
for i in itertools.repeat("hola"):
 print(i)
 contador+=1
 if contador==5: # Sólo mostramos 5 elementos
  break

# Si utilizamos un segundo parámetro será el número de veces
# que se repita
import itertools
for i in itertools.repeat("hola",5):
 print(i)

# La función accumulate va devolviendo la suma de todos los
# elementos de un objeto iterable
# Puesto que todos los elementos de la list son iguales a uno
# La suma total se incrementará en uno por cada elemento
# Resultado = [1, 2, 3, 4, 5]
from itertools import accumulate
miLista=[1,1,1,1,1]
print(list(accumulate(miLista))) #Resultado: [1, 2, 3, 4, 5]

# Para generar una lista del mismo elemento repetido varias veces
# podríamos utilizar el método repeat
# Este ejemplo es idéntico al anterior
import itertools
print(list(accumulate(itertools.repeat(1,5)))) #Resultado: [1, 2, 3, 4, 5]

# Esta vez utilizaremos como objeto iterable una lista de los
# números del 1 al 9, y mostraremos su acumulado
# Recordemos que el segundo parámetro de range es el valor 
# máximo excluido
import itertools
print(list(accumulate(range(1,10)))) #Resultado = [1, 3, 6, 10, 15, 21, 28, 36, 45]
