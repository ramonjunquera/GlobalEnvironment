#Autor: Ramón Junquera
#Fecha: 20221221

# Decoradores
# Permiten crear nuevas funciones modificando otras existentes

# Comenzamos con un ejemplo normal
# Definimos una función que imprime un texto, pero antes y después
# escribe una línea con tantos guiones como la longitud del texto
def imprime(texto):
  rayas="-"*len(texto)
  print(rayas+texto+rayas)
imprime("hola") #Resultado: ----hola----

# Cada vez que se quiera utilizar esta función tendremos que pasar
# el parámetro con el texto
# Lo que haremos ahora será crear una nueva función que no
# necesite parámetros y que escriba un texto fijo llamando a la
# función imprime
def imprime(texto):
  rayas="-"*len(texto)
  print(rayas+texto+rayas)
# Mi nuevo objeto es la llamada a una función con un parámetro fijo ya definido
miFuncion=imprime("hola")
# Llamo a mi nuevo objeto
miFuncion #Resultado: ----hola----
