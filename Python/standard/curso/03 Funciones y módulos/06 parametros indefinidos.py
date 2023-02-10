#Autor: Ramón Junquera
#Fecha: 20230104

# El número de parámetros de llamada a una función puede ser indefinido
# Para ellos del parámetro que lo representa comienza por un *^
# Ese parámetro contendrá una tupla con con todos los parámetros pasados.
# Los parámetros variable siempre irán al final, qudando el siguiente orden:
#  - parámetros fijos
#  - parámetros con valores por defecto
#  - parámetros variables

def myFunction(*params):
    print("Tenemos {} parámetros".format(len(params)))
    index=0
    for param in params:
        print(index,param)
        index+=1

myFunction("Pedro","Pérez")
myFunction("uno","dos","tres")

# Los parámetros variables pueden pasarse como nombre, apra ello necesitaremos un parámetro de
# tipo diccionario y no tupla. Para ellos el nombre del parámetro variable debe ir precedido de **

def myFunction(**params):
    print("Tenemos {} parámetros".format(len(params)))
    for param in params:
        print(param,params[param])

myFunction(nombre="Pedro",apellido="Pérez")
myFunction(primero="uno",segundo="dos",tercero="tres")
