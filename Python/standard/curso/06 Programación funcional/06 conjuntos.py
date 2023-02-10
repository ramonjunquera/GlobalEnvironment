#Autor: Ramón Junquera
#Fecha: 20221221

# Conjuntos
# Es una estructura de datos similar a las listas, pero que no
# permite elementos duplicados. Además no se tiene en cuenta el
# orden de los elementos.
# Realmente es un diccionario de claves sin valores.
# Como en cualquier diccionario, las claves están indexadas.

# Podemos definir un conjunto con llaves, como un diccionario, pero
# con un sólo valor en cada elemento
miConjunto={1,2,3,4,5}
print(miConjunto) #Resultado: {1, 2, 3, 4, 5}

# También podemos hacer conjuntos de cadenas
miConjunto={"uno","dos","tres"}
print(miConjunto) #Resultado: {'uno', 'dos', 'tres'}

# Se puede crear un conjunto vacío
miConjunto={}
print(miConjunto) #Resultado: {}

# También podemos utilizar la función set() para convertir una
# lista en un conjunto
miLista=[1,2,3,4,5]
miConjunto=set(miLista)
print(miConjunto) #Resultado: {1, 2, 3, 4, 5}

# Si intentamos introducir más de una vez el mismo elemento, no
# nos dará error, pero no lo permitirá. Sólo se guadará una vez
miConjunto={1,3,2,1,2,3,3,3,2,1,3,1}
print(miConjunto) #Resultado: {1, 2, 3}

# Podemos utilizar el comando in para comprobar si un conjunto 
# contiene un determinado elemento
# Respuesta = True
miConjunto={1,2,3,4,5}
print(4 in miConjunto) #Resultaso: True

# Como las listas, podemos obtener el número de elementos con
# la función len
miConjunto={1,2,3,4,5}
print(len(miConjunto)) #Resultado: 5

# Puesto que los elementos de un conjunto están indexados y los 
# de una lista no, es más rápido comprobar la existencia de un
# elemento en un conjunto que en una lista

# Podemos añadir un elemento más a un conjunto con el método add
miConjunto={1,2,3,4,5}
miConjunto.add(6)
print(miConjunto) #Resultado: {1, 2, 3, 4, 5, 6}

# Se puede eliminar un elemento de un conjunto con el método
# remove
miConjunto={1,2,3,4,5}
miConjunto.remove(3)
print(miConjunto) #Resultado: {1, 2, 4, 5}
# Si se intenta eliminar un elemento inexistente se producirá
# una excepción

# El método pop() extrae un elemento aleatorio del conjunto y 
# devuelve su valor
miConjunto={1,2,3,4,5}
x=miConjunto.pop()
print("Extraido elemento {}".format(x)) #Resultado: 1
print("Conjunto actual {}".format(miConjunto)) #Resultado: {2, 3, 4, 5}

# Ejemplo que extrae uno a uno todos los valores de un conjunto
miConjunto={1,2,3,4,5}
while len(miConjunto)>0:
 print("Extraido elemento {}. Conjunto actual {}".format(miConjunto.pop(),miConjunto))
 