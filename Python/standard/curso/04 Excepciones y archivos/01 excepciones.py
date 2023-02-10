#Autor: Ramón Junquera
#Fecha: 20221219

# Cuando intentamos calcular una operación ilegal o usar un 
# código incorrecto, obtenemos un error con su correspondiente
# mensaje que detiene la ajecución del programa. Esto es una excepción.
# En el siguiente ejemplo obtenemos una exceptción del tipo
# ZeroDivisionError
#print(7/0)
# El siguiente produce una excepción del tipo TypeError
#print("hola"+3)

# Las excepciones pueden ser controladas con la sentencia
# try/except
# Cualquier excepción producida en la sección try
# que corresponda con el tipo de excepción definido en except
# saltará automáticamente a la sección except
# En el siguiente ejemplo se produce una excepción del tipo
# ZeroDivisionError
try:
 a=7
 b=0
 print(a/b)
 print("Cálculo finalizado") # Nunca se ejecutará
except ZeroDivisionError:
 print("Se ha intentado dividir entre cero")

# En el siguiente ejemplo no se producen excepciones
try:
 print(10/2)
 print("Cálculo finalizado")
except ZeroDivisionError:
 print("Se ha intentado dividir entre cero") # Nunca se ejecuta
print("Programa terminado")

# Una sentencia try puede tener más de una sección except para
# gestionar los distintos tipos de excepciones
try:
  a=10
  print(a+'hola')
  print(a/2)
except ZeroDivisionError:
 print("Se ha intentado dividir entre cero")
except TypeError:
 print("Se ha producido un error de tipos")

# Incluso uns sección except puede gestionar varios tipos a la vez
try:
 print(3/0)
 print("Cálculo finalizado")
except(ZeroDivisionError,TypeError):
 print("Tenemos un error")
print("Programa terminado")

# Si la sección except no tiene definido un tipo de excepción
# servirá para cualquiera
try:
 print(3/0)
 print("Cálculo finalizado")
except:
 print("Tenemos un error desconocido")
print("Programa terminado")

# Si dejamos el except sin definición de tipo de excepción para el
# final, podremos controlar las excepciones conocidas y las 
# desconocidas
try:
 print(3/0)
 print("Cálculo finalizado")
except ZeroDivisionError:
 print("Se ha intentado dividir entre cero")
except:
 print("Tenemos un error desconocido")
print("Programa terminado")
