#Autor: Ramón Junquera
#Fecha: 20221219

# Las funciones pueden ser tratados como objetos
# Y pueden ser asignadas a variables
# Definimos una función que devuelva el producto de sus 2 parámetros
def producto(x,y):
 return x*y
# Creamos una variable (objeto) con la función definida
multiplica=producto
# A partir de ahora podremos utilizar la variable como la función
print(multiplica(5,8))

# El resultado de una función se puede utilizar como argumento
# de otra

def suma(a,b):
 return a+b
print(suma(5,suma(4,3))) #Resultado: 5+(4+3)=5+7=12

# Incluso podemos pasar una función como parámetro
def doble(a):
 return a*2
def llamaFuncion(functionName,param1):
 return functionName(param1)
print(llamaFuncion(doble,4)) #Resultado: 8