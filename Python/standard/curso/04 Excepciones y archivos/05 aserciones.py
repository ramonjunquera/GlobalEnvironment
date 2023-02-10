#Autor: Ramón Junquera
#Fecha: 20221219

# Una aserción es una comprobación de validez utilizada para 
# generar excepciones
# Se crean con el comando assert y se activan si no se cumple
# su condición (cuando son False)

# Ejemplo de uso de aserciones
print(1)
assert 2+2==4 # Como es True no pasa nada
print(2)
assert 1+1==3 # Salta una excepción del tipo AssertionError
print(3) # Nunca llega a ejecutarse

# Las excepciones generadas por aserciones pueden tener un texto
# descriptivo
temperatura = -10
assert temperatura > 0,"La temperatura siempre debe ser mayor que cero"
print(temperatura)

# Las excepciones generadas por aserciones pueden ser capturadas
# por estructuras try/except/finally
try:
 temperatura=-10
 assert temperatura > 0
 print("Temperatura correcta")
except AssertionError:
 print("Tenemos una excepción por aserción")
print("Fin del programa")