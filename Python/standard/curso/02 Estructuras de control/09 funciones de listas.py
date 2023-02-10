#Tema: Funciones de listas
#Fecha: 20230124
#Autor: Ramón Junquera

# Se puede añadir un nuevo elemento al final de una lista con el
# método append
miLista = [1,2,3]
miLista.append(4)
print(miLista) #Resultado [1, 2, 3, 4]
# El elemento a añadir podría ser otra lista
# El siguiente ejemplo añade una lista como último elemento
# Al final lista1 tendrá 4 elementos. Los 3 primeros serán enteros y el último
# una lista de 3 elementos enteros
lista1=[1,2,3]
lista2=[4,5,6]
lista1.append(lista2)
print(lista1) #Resultado: [1, 2, 3, [4, 5, 6]]
# Si lo que se quiere es añadir varios elementos debemos utilizar la instrucción extend()
# El parámetro será una lista/tupla de los nuevos elementos
lista1=[1,2,3]
lista2=(4,5,6)
lista1.extend(lista2)
print(lista1) #Resultado: [1, 2, 3, 4, 5, 6]

# Podemos obtener el número de elementos de una lista con la
# función len, como con las cadenas
miLista=['uno','dos','tres']
print('miLista tiene ' + str(len(miLista)) + ' elementos')
miLista.append('cuatro')
print('Ahora tiene ' + str(len(miLista)) + ' elementos')

# Se puede insertar un nuevo elemento en una posición concreta
miLista=[1,2,3,5,6]
# Insertamos en la posición 3 el elemento 4
miLista.insert(3,4)
# Ahora toda la lista estará en orden
print(miLista) #Resultado: [1, 2, 3, 4, 5, 6]

# Podemos obtener el número de veces que aparece un elemento
# dentro de una lista con el método count
miLista=[1,2,3,1,4,2,2,5]
print('El 2 aparece ' + str(miLista.count(2)) + ' veces')

# El método index indica la primera posición en la que encuentra
# un determinado elemento en una lista
miLista=['uno','dos','tres','dos']
print('La palabra "dos" aparece en la posicion ' + str(miLista.index('dos')))
# Respuesta: La palabra "dos" aparece en la posicion 1
# El método index también admite 3 parámetros con la siguiente sintaxis
#   index(elemento,indice_inicio,indice_fin)
# El siguiente ejemplo muestra la primera posición en la que aparece el elemento 3
# entre las posiciones 4 y 6
miLista=[1,3,2,4,2,3,1,0,8,2]
print(miLista.index(3,4,6)) #Resultado: 5

# La función max muestra el mayor elemento de una lista
miLista=[1,2,3,1,4,2,2,5,3,4]
print(max(miLista)) #Resultado: 5
# Si la lista tiene cadenas, el orden será alfabético
# Resultado = 'uno'
miLista=['uno','dos','tres','cuatro']
print(max(miLista)) #Resultado: uno
# La función max dará un error si la lista contiene elementos de cadena y numéricos
# al mismo tiempo

# También existe la función min para obtener el mínimo valor de una lista
# Resultado = -3.4
miLista=[3,7.3,-2,9,-3.4,0,1,66]
print(min(miLista)) #resultado: -3.4

# El método remove elimina de una lista el primer elemento que coincida con
# uno dado
miLista=[1,2,3,4,5]
miLista.remove(3)
print(miLista) #Resultado: [1, 2, 4, 5]
# Si la lista tiene más de un elemento igual, sólo eliminará el primero
miLista=[1,2,3,1,2,3]
miLista.remove(3)
print(miLista) #Resultado: [1, 2, 1, 2, 3]
# Si se pide eliminar un elemento inexistente da un error

# El programa elimina un elemento tantas veces como exista en una lista
elemento_a_borrar=3
miLista=[1,2,3,1,2,3]
while miLista.count(elemento_a_borrar)>0:
  miLista.remove(elemento_a_borrar)
print(miLista) #Resultado: [1, 2, 1, 2]

# El método pop sin parámetros elimina el último elemento de una lista y devuelve el
# elemento eliminado
miLista=[1,2,3,4,5]
elementoEliminado=miLista.pop()
print(miLista) #Resultado: [1, 2, 3, 4]
print(elementoEliminado) #Resultado: 5
# Si al método pop le pasamos un parámetro será el índice del elemento a borrar
miLista=[1,2,3,4,5]
miLista.pop(2)
print(miLista) #Resultado: [1, 2, 4, 5]

# El método reverse invierte el orden de los elementos de una lista
miLista=["uno","dos","tres","cuatro"]
miLista.reverse()
print(miLista) #Resultado: ['cuatro', 'tres', 'dos', 'uno']

# El método sort ordena de forma ascendente una lista
miLista=[3,5,1,4,2]
miLista.sort()
print(miLista) #Resultado: [1, 2, 3, 4, 5]
# Para ordenarlo de forma descendente le pasamos al método sort el parámetro reverse=True
miLista=[3,5,1,4,2]
miLista.sort(reverse=True)
print(miLista) #Resultado: [5, 4, 3, 2, 1]
