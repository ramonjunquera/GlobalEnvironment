#Autor: Ramón Junquera
#Fecha: 20230104

# Se pueden dar valores por defecto a los parámetros de una función.
# Así, ,cuando se llame, no será obligatorio indicar todos ellos.
# Los parémetros con valores por defecto siempre deben ir al final.
def suma(a,b=1):
    return a+b
print(suma(2,3)) #Resultado: 5
print(suma(2)) #Resultado: 3
