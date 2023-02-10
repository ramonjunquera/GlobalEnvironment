#Autor: Ramón Junquera
#Fecha: 20221220

# Cortes de lista (o tuplas)
# Cuando mostramos un elemento de una lista hacemos referencia a él
# mediante su índice.
# En vez de utilizar un índice concreto, podemos usar un rango.
# El rango está formado por un índice inicial y uno final
# separados por dos puntos.
# Como en los rangos, el indice inicial está incluido, pero
# el final no.
# Para mostrar el ejemplo crearemos una lista con los mismos
# valores que sus índices.
miLista=[0,1,2,3,4,5,6,7,8,9]
# Mostramos los elementos con índices desde el 3 hasta el 6
# El último valor indicado nunca entra en el rango
print(miLista[3:7])

# Si omitimos alguno de los indices del rango se tomará el resto
# Si omitimos el índice inicial, se comenzará desde el principio
miLista=[0,1,2,3,4,5,6,7,8,9]
# Mostramos los elementos con índices desde el 0 hasta el 6
# El último valor indicado nunca entra en el rango
print(miLista[:7])

# Si omitimos el índice final se tomará desde el índice inicial
# hasta el final de la lista
miLista=[0,1,2,3,4,5,6,7,8,9]
print(miLista[5:]) #Respuesta: [5, 6, 7, 8, 9]

# Si añadimos un tercer valor al rango (también separado por dos
# puntos) estaremos indicando el incremento a tener en cuenta para
# pasar desde el indice inicial al final
# Cuando no se pone, se toma por defecto 1
# En el siguiente ejemplo sólo mostraremos los elementos que
# se encuentran en posiciones pares comenzando desde la primera
# (la cero) y hasta el final
miLista=[0,1,2,3,4,5,6,7,8,9]
print(miLista[::2]) #Respuesta: [0, 2, 4, 6, 8]

# Una forma habitual de mostrar los elementos de una lista en
# orden inverso es no indicar el indice inicial ni final, pero 
# como tomar como incremento -1
miLista=[0,1,2,3,4,5,6,7,8,9]
print(miLista[::-1]) #Respuesta: [9, 8, 7, 6, 5, 4, 3, 2, 1, 0]

# Si utilizamos valores negativos para los índices estaremos
# indicando que el elemento debe ser contado desde el final
# Ejemplo: muestra todos los elementos de la lista excepto
# el último
miLista=[0,1,2,3,4,5,6,7,8,9]
print(miLista[:-1]) #Respuesta: [0, 1, 2, 3, 4, 5, 6, 7, 8]

# Muestra todos los elementos de la lista excepto el primero
# y el último
miLista=[0,1,2,3,4,5,6,7,8,9]
print(miLista[1:-1]) #Respuesta: [1, 2, 3, 4, 5, 6, 7, 8]
