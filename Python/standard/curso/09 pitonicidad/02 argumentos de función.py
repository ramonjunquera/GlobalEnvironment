#Autor: Ramón Junquera
#Fecha: 20221223

# Pitonicidad. Múltiples argumentos de función

# Hasta ahora, las funciones o métodos siempre tenían un determinado
# número de argumentos.
# Podemos definir una función con un número indeterminado de argumentos
# En el ejemplo siguiente se sustituyen todos los argumentos por la 
# variable *args que representa una tupla dentro de la función
# La función muestra el número de elementos y cada uno de ellos por separado
def miFuncion(*args):
 print("Número de argumentos: {}".format(len(args)))
 for i in args:
  print(i)
miFuncion("uno",2,3.3,"cuatro",5000)
# Habitualmente a los argumentos variables se les da el nombre de *args
# pero sólo es una convención. Podría tener cualquier nombre.

# Si no todos los argumentos son variables, porque hay algunos obligatorios
# es necesario dejar para el final la variable *args
# En ese caso, *args contendrá el resto de parámetros desde ese punto hasta
# el final
# Se acepta que args represente una tupla vacía (sin argumentos)
def miFuncion(a1,a2,*args):
 print("argumento 1 = {}".format(a1))
 print("argumento 2 = {}".format(a2))
 for i in range(len(args)):
  print("argumento {} = {}".format(i+3,args[i]))
miFuncion(1,2,3,4,5)
miFuncion(1,2,3)
miFuncion(1,2)

# Podemos hacer que los parámetros de las funciones sean optativos
# Para ello tendremos que dar un valor por defecto al parámetro en
# la definición de la función.
# Si no se incluye ese parámetro, se le dará el valor por defecto
def miFuncion(a1,a2,a3="hola"):
 print("argumento 1 = {}".format(a1))
 print("argumento 2 = {}".format(a2))
 print("argumento 3 = {}".format(a3))
miFuncion(2,3)
miFuncion(4,5,"hola")
miFuncion(1.2,"hola","adios")
# De esta manera, todos los parámetros optativos deben ir siempre al final
# o dicho de otro modo, no se puede poner un argumento obligatorio tras uno
# optativo

# Podemos trabajar con un número indeterminado de parámetros y con
# parámetros optativos
def miFuncion(a1,a2="segundo",*args):
 print("argumento 1 = {}".format(a1))
 print("argumento 2 = {}".format(a2))
 for i in range(len(args)):
  print("argumento {} = {}".format(i+3,args[i]))
miFuncion(1,2,3,4,5)
miFuncion(1,2,3)
miFuncion(1,2)
miFuncion(1)

# Además de lo anterior podríamos asignar nombre a las variables que pasamos
# en la llamada a la función.
# Esto nos permite gestionar las variables por nombre y no por posición
# Para ello es necesario añadir a la definición de la función el parámetro
# **kwargs (es una convención).
# La variable kwargs se convertirá en un diccionario con los nombres (claves)
# y valores de los parámetros variables con nombre asignado
# Si no tienen nombre asignado seguirán apareciendo en la tupla args
# Juntando todo tendríamos
def miFuncion(a1,a2="segundo",*args,**kwargs):
 print("argumento 1 = {}".format(a1))
 print("argumento 2 = {}".format(a2))
 for i in range(len(args)):
  print("argumento {} = {}".format(i+3,args[i]))
 print(kwargs)
miFuncion(1,2,3,4,5)
# Se muestran los parámetros tál y como los hemos escrito, porque a ninguno 
# le hemos dado nombre
miFuncion(1,2,3,4,a55=55)
# El parámetro a1 tendrá el valor 1
# El parámetro a2 tendrá el valor 2, porque no se ha omitido
# La tupla args contendrá los valores 3 y 4, que son los parámetros
# variables sin nombre
# El diccionario kwargs contendrá la pareja a55:55 que es el único
# parámetro variable con nombre
miFuncion(1,2,a33=33,a44=44,a55=55)
# El parámetro a1 tendrá el valor 1
# El parámetro a2 tendrá el valor 2, porque no se ha omitido
# La tupla args estará vacía porque todos los parámetros variables
# tienen nombre asignado
# El diccionario kwargs contendrá 3 elementos para los 3 últimos
# parámetros que son variables y tiene nombre asignado
miFuncion(1,a22=22,a33=33,a2=5)
# El parámetro a1 tendrá el valor 1
# El parámetro a2 tendrá el valor 5, porque uno de los parámetros 
# variables tiene el nombre de este parámetro y se le asigna su valor
# La tupla args estará vacía porque todos los parámetros variables
# tienen nombre asignado
# El diccionario kwargs contendrá las claves a a22 y a33 porque todos
# los parámetros variables tienen nombres
miFuncion(1,a22=22)
# El parámetro a1 tendrá el valor 1
# El parámetro a2 tendrá el valor "segundo" porque no se le ha asignado valor
# La tupla args estará vacía porque todos los parámetros variables
# tienen nombre asignado
# El diccionario kwargs contendrá la clave a22
miFuncion(1)
# El parámetro a1 tendrá el valor 1
# El parámetro a2 tendrá el valor "segundo" porque no se le ha asignado valor
# La tupla args estará vacía porque no hay parámetros variables
# El diccionario kwargs estará vacío porque no hay parámetros con nombre
