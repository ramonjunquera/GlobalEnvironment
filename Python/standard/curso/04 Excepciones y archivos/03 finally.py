#Autor: Ramón Junquera
#Fecha: 20221219

# La sección finally es la última de try/except
# Es optativa
# El código de esta sección siempre se ejecuta, tanto si hay
# excepciones como si no

# En el siguiente ejemplo se produce una excepción que es tenida
# en cuenta por la sección except. Finalmente se ejecuta siempre
# el código de finally
try:
 print("Hola")
 print(1/0)
except ZeroDivisionError:
 print("Se ha intentado dividir por cero")
finally:
 print("Siempre se ejecutará este código")

# Si se produce algún error en una sección que no sea la try
# producirá la excepción correspondiente
# En el siguiente ejemplo se intenta mostrar una variable no
# definida previamente, y se produce una excepción
try:
 print("Hola")
 print(1/0)
except ZeroDivisionError:
 print("Se ha intentado dividir por cero")
 print(variable_desconocida) #Provoca un error!
finally:
 print("Siempre se ejecutará este código")
 