#Autor: Ramón Junquera
#Fecha: 20221221

# map y filter
# Son funciones que se aplican a objetos iterables (listas o tuplas)

# map permite aplicar una función a cada uno de los objetos de
# una lista (o tupla) y devuelve otro objeto similar en el 
# que sus objetos ya han sido modificados
# En el siguiente ejemplo definimos una función que suma 5 al
# valor pasado como parámetro
# Definimos una lista con valores numéricos
# Con la función map aplicamos la función de sumar 5 a cada uno 
# de los elementos de la lista
def suma5(x):
 return x+5
miLista=[10,20,30,40,50]
print(list(map(suma5,miLista))) #Resultado: [15, 25, 35, 45, 55]

# Se podría hacer lo mismo con una función lambda
miLista=[10,20,30,40,50]
print(list(map(lambda x:x+5,miLista))) #Resultado: [15, 25, 35, 45, 55]

# filter permite seleccionar sólo los elementos que cumplan
# cierta condición
def esPar(x):
 return x%2==0
miLista=[11,22,33,44,55]
print(list(filter(esPar,miLista))) #Resultado [22, 44]

# Con una función lambda sería
miLista=[11,22,33,44,55]
print(list(filter(lambda x:x%2==0,miLista))) #Resultado [22, 44]
