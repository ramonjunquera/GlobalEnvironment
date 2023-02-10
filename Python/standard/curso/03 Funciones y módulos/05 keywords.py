#Autor: Ramón Junquera
#Fecha: 20230104

# Al llamar a una función, se pueden identificar los nombres de los parámetros y así no seguir
# exactamente el mismo orden con el que se difinió la función

def nombreCompleto(nombre,apellido):
    print(nombre,apellido)
nombreCompleto("Pedro","Pérez")
nombreCompleto(apellido="Pérez",nombre="Pedro")
