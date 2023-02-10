#Autor: Ramón Junquera
#Fecha: 20221219

# Las excepciones se pueden generar desde código
# Recordemos que una excepción es un error que detiene el 
# flujo del proceso
# Una excepción se genera con el comando raise

# El siguiente ejemplo genera una excepción del tipo ValueError
# cuyo tipo existe realmente
print('hola')
#raise ValueError
print('adiós') # Nunca llega hasta aquí

# Como en ejemplos anteriores, podemos generar una excepción en
# una estructura try/except/finally
try:
 print("hola")
 raise ZeroDivisionError
 print("adiós") # No se ejecutará
except ZeroDivisionError:
 print("Se ha intentado dividir por cero")
finally:
 print("Estoy en la sección finally")

# También podemos generar una nueva excepción en una sección que
# no sea la try
try:
 print("hola")
except ZeroDivisionError:
 print("Se ha intentado dividir por cero")
finally:
 raise ZeroDivisionError # Generación de excepción
 print("Estoy en la sección finally") # Código inalcanzable

# Se puede añadir un texto descriptivo del error de la excepción
# cuando se genera
print("hola")
raise ZeroDivisionError("Se ha intentado dividir por cero")
print("adiós") # Nunca se ejecuta
