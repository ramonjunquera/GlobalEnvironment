#Autor: Ramón Junquera
#Fecha: 20221219

# Definición de funciones

# Las funciones deben ser definidas antes de ser llamadas

# Definición de función simple sin parámetros
def saluda():
 print("hola")
saluda() # Llamamos a la función

# Definimos una función que muestra el doble del primer parámetro
def doble(a):
 print(a*2)

doble(3) # Llamamos a la función

# Ejemplo de función que imprime si el número es par o impar
def esPar(n):
 if n%2==0:
  print("si")
 else:
  print("no")
# Llamamos a la función
esPar(7) #Resultado: no
esPar(10) #Resultado: si

# Función que imprime una palabra un número de veces
def concatena(palabra,veces):
 print(palabra*veces)
concatena("hola",3) # Llamamos a la función
