#Autor: Ramón Junquera
#Fecha: 20221220

# Todas las funciones que no devuelven valor realmente entregan
# la clausula None, que significa ausencia de valor.

# Definimos una función que no devuelve nada
def miFuncion():
  a=3 # No es importante lo que haga
# Mostramos el valor que devuelve
print(miFuncion()) #Respuesta: None 

# También puede asignar el valor None a una variable
# La variable existirá, pero no tendrá valor
a=None
print(a)

# Incluso se puede comparar
a=None
if a==None:
  print("Es None")
else:
  print("No es None")
