#Autor: Ramón Junquera
#Fecha: 20221219

# La función range crea una lista secuencial de números enteros
# Si sólo se le pasa un parámetro, comenzará en 0 y tendrá el número de elementos
# indicados.
# Por sí mismo un rango es una secuencia no una lista. Para convertirlo en lista
# necesitaremos la función list
numeros=list(range(10))
print(numeros) #Resultado: [0, 1, 2, 3, 4, 5, 6, 7, 8, 9]
# Si le pasamos dos parámetros el primero indicará el valor inicial y el segundo
# el último excluido. Uno más que el último del rango
numeros=list(range(3,9))
print(numeros) #Resultado: [3, 4, 5, 6, 7, 8]
# Si se le pasan 3 parámetros se entenderá que el tercero será el incremento
numeros=list(range(5,20,2))
print(numeros) #Resultado: [5, 7, 9, 11, 13, 15, 17, 19]
# Los parámetros de la función range siempre deben ser números enteros, sino dará error