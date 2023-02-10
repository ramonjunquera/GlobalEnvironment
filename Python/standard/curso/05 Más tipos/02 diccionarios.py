#Autor: Ramón Junquera
#Fecha: 20221220

# Un diccionario es una estructura de datos que almacena los
# valores por pares. El primero de ellos es la clave y el segundo
# el valor asociado a esa clave.

# Se definen de manera similar a las listas pero en vez de corchetes
# se utilizan llaves
miDicc={"Koko":"Marron","Blacky":"Negro","Nieve":"Blanco"}
print(miDicc)

# Un diccionario no puede tener claves repetidas
# Si repetimos clave, simplemente se actualizará el valor
# Si asignamos los valores en una línea y tenemos claves repetidas
# se tomará el valor de la última clave
# Resultado: la clave "Koko" tiene el valor "Blanco"
miDicc={"Koko":"Marron","Blacky":"Negro","Koko":"Blanco"}
print(miDicc)

# No hay problema en que un diccionario tenga valores duplicados
miDicc={"Koko":"Marron","Canela":"Marron","Blacky":"Negro"}
print(miDicc)

# Las claves de un diccionario deben ser indexables: valores 
# numéricos, booleanos o cadenas.
# No se permiten otros tipos (listas, ...)
miDicc={1:"uno",2:"dos",3:"tres"}
print(miDicc)
miDicc={1.5:"uno",2.7:"dos",3.2:"tres"}
print(miDicc)
miDicc={True:"verdad",False:"mentira"}
print(miDicc)

# Las claves de un diccionario pueden ser de distinto tipo
miDicc={1:3,2.5:4.5,"cuatro":4,False:8}
print(miDicc)

# Se puede crear un diccionario vacío si sólo dejamos las llaves
miDicc={}
print(miDicc)

# Los valores de un diccionario pueden ser de distintos tipos
miDicc={1:3,2:4.5,3:[3,4,5],4:"cuatro"}
print(miDicc)

# Los datos se recuperan por clave
miDicc={"Koko":"Marron","Blacky":"Negro","Nieve":"Blanco"}
print(miDicc["Blacky"])
RGB={'red':[255,0,0],'green':[0,255,0],'blue':[0,0,255]}
print(RGB["red"])
