#Autor: Ramón Junquera
#Fecha: 20221220

# Listas por comprensión
# Hasta ahora rellenabamos inicialmente una lista en una primera
# línea en la que le asignábamos los valores.
# Si los valores de una lista tienen alguna lógica y pueden ser
# generados con alguna fórmula, podemos escribir la fórmula
# en vez de los valores

# Crearemos una lista que comienza en 0, contiene sólo números pares
# ascendentes y finaliza en el 8
miLista=[i*2 for i in range(5)]
print(miLista) #Respuesta: [0, 2, 4, 6, 8]

# Lista con los cuadrados desde el 2 al 9
miLista=[i**2 for i in range(2,10)]
print(miLista) #Respuesta: [4, 9, 16, 25, 36, 49, 64, 81]

# A la fórmula que genera la lista se le puede añadir al final
# una condición que se debe cumplir para que el valor sea añadido
# Creamos una lista con los pares desde el 0 al 8
miLista=[i for i in range(9) if i%2==0]
print(miLista) #Respuesta: [0, 2, 4, 6, 8]

# Lista con todos los cuadrados de los números del 1 al 9 que
# son pares
miLista=[i**2 for i in range(1,10) if i**2 % 2==0]
print(miLista) #Respuesta: [4, 16, 36, 64]

# Lista con los múltiplos de 3 desde 0 hasta 20
miLista=[i for i in range(20) if i%3==0]
print(miLista) #Respuesta: [0, 3, 6, 9, 12, 15, 18]

# La lista no se completará hasta que no se hayan calculado todos
# sus elementos. Después la lista necesitará tanta memoria como
# para almacenar todos los elementos calculados.
# Si el número de elementos es muy alto es posible que nos quedemos
# sin memoria.
# Por ejemplo, generamos una lista que contenga todos los números
# desde el 0 al 10^100
miLista=[i for i in range(10**100)]
# Obtenemos un error de OverflowError
# Estos casos no es una buena idea utilizar una lista
