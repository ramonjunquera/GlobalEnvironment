#Autor: Ramón Junquera
#Fecha: 20230124

# Se pueden añadir elementos a un diccionario existente
cuadrados={1:1,2:4,3:9}
cuadrados[5]=25
print(cuadrados)

# Los valores de los elementos se sobreescriben de la misma manera
miDicc={1:1,2:2,3:3}
miDicc[2]="dos"
print(miDicc)

# Podemos saber si un diccionario tiene una clave con el comando in
miDicc={1:1,2:2,3:3}
if 5 in miDicc:
  print("Está en el diccionario")
else:
  print("No está en el diccionario") #*

# Si pedimos el valor de una clave inexistente provocaremos una
# excepción, pero si utilizamos el método get y no existe la clave
# nos devolverá por defecto el valor None
miDicc={1:"uno",2:"dos",3:"tres"}
print(miDicc.get(2)) # Devuelve "dos"
print(miDicc.get(5)) # Devuelve None

# Un diccionario puede contener None como clave
# Incluso podríamos pedir el valor de la clave None
# En este caso, el comportamiento de get es el mismo
miDicc={1:"uno",2:"dos",3:"tres",None:99}
print(miDicc.get(2)) # Devuelve "dos"
print(miDicc.get(None)) # Devuelve 99
print(miDicc.get(5)) # Devuelve None

# Si al método get le añadimos un segundo parámetro, será ese
# el valor que devuelva por defecto cuando no encuentre la clave
# en vez de None
miDicc={1:"uno",2:"dos",3:"tres"}
print(miDicc.get(5,99)) # Resultaso: 99

# Borramos un elemento del diccionario por su clave
miDicc={1:"uno",2:"dos",3:"tres"}
del miDicc[2]
print(miDicc) #Resultado: {1: 'uno', 3: 'tres'}

# Borramos todos los elementos de un diccionario
miDicc={1:"uno",2:"dos",3:"tres"}
miDicc.clear()
print(miDicc) #Resultado: {}

# Borramos el diccionario completo (el objeto)
miDicc={1:"uno",2:"dos",3:"tres"}
del miDicc
# A partir de ahora la variable miDicc no será reconocida

# La signación de variables de diccionario siempre se hace por referencia, no por valor
# Cuando asignamos la variable de un diccionario a otra, simplemente creamos un nuevo
# puntero al mismo diccionario.
# Las modificaciones en el diccionario  podrán ser vistan desde ambas variables
miDicc={1:"uno",2:"dos",3:"tres"}
miDicc1=miDicc
miDicc1[2]="el dos"
print(miDicc) #Resultado: {1: 'uno', 2: 'el dos', 3: 'tres'}
print(miDicc1) #Resultado: {1: 'uno', 2: 'el dos', 3: 'tres'}

# Si queremos hacer una asignación por valor (crear un duplicado del diccionario), necesitamos
# utilizar el comando copy
miDicc={1:"uno",2:"dos",3:"tres"}
miDicc1=miDicc.copy()
miDicc1[2]="el dos"
print(miDicc) #Resultado: {1: 'uno', 2: 'dos', 3: 'tres'}
print(miDicc1) #Resultado: {1: 'uno', 2: 'el dos', 3: 'tres'}

# Podemos crear un diccionario cuyas claves se las pasamos en una lista/tupla
# Ninguna clave tendrá valor
miTupla=("uno","dos","tres")
miDicc=dict.fromkeys(miTupla)
print(miDicc) #Resultaso: {'uno': None, 'dos': None, 'tres': None}

# El método fromkeys admite un segundo parámetro que será el valor por defecto para
# todas sus claves
miTupla=("uno","dos","tres")
miDicc=dict.fromkeys(miTupla,123)
print(miDicc) #Resultaso: {'uno': 123, 'dos': 123, 'tres': 123}

# Podemos añadir nuevas claves con sus valores de un diccionario a otro
miDicc1={1:"uno",2:"dos"}
miDicc2={3:"tres",4:"cuatro"}
miDicc1.update(miDicc2)
print(miDicc1) #Resultado: {1: 'uno', 2: 'dos', 3: 'tres', 4: 'cuatro'}
#Si alguno de los elementos existe, se sobreescribirá
miDicc1={1:"uno",2:"dos"}
miDicc2={2:"el dos",3:"tres"}
miDicc1.update(miDicc2)
print(miDicc1) #Resultado: {1: 'uno', 2: 'el dos', 3: 'tres'}

# Podemos añadir una nueva clave con su valor si es que aun no existe en el diccionario
# Si existe la clave, no hará nada
miDicc={1:"uno",2:"dos"}
miDicc.setdefault(2,"el dos") 
print(miDicc) #Resultaso: {1: 'uno', 2: 'dos'}
miDicc.setdefault(3,"tres")
print(miDicc) #Resultaso: {1: 'uno', 2: 'dos', 3: 'tres'}

# Podemos obtener cada uno de los elementos de un diccionaro por iteracción
miDicc={1:"uno",2:"dos",3:"tres"}
for key,value in miDicc.items():
  print("{} : {}".format(key,value))

# Podemos obtener el listado de claves de un diccionario
miDicc={1:"uno",2:"dos",3:"tres"}
misKeys=miDicc.keys()
print(misKeys) #Respuesta: dict_keys([1, 2, 3])
for key in misKeys:
  print(key)

# También podemos obtener el listado de valores de un diccionario
miDicc={1:"uno",2:"dos",3:"tres"}
misValues=miDicc.values()
print(misValues) #Respuesta: dict_values(['uno', 'dos', 'tres'])
for value in misValues:
  print(value)

# Al igual que las listas/tuplas, podemos obtener el número de elementos
miDicc={1:"uno",2:"dos",3:"tres"}
print(len(miDicc)) #Resultado: 3