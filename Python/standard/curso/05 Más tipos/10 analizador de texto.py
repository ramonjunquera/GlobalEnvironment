#Autor: Ramón Junquera
#Fecha: 20221220

# Ejemplo de analizador de texto
# Dado un texto contenido en una variable se mostrará el
# porcentaje de aparición de cada una de las letras del
# abecedario

# Definimos la variable con el texto a analizar
# Utilizamos triples comillas para escribir texto multilínea
# más fácilmente
texto="""hola esto es una prueba
para contar el numero de letras que aparecen
y calcular el porcentaje
Para simplificarlo no tiene ni mayusculas ni caracteres especiales
como acentos ni signos de puntuacion"""

# Definimos una función que contará el número de veces que
# aparece una carácter en una cadena
def count_char(text,ch):
  count=0
  for c in text:
    if c==ch:
      count+=1
  return count

# Definimos el abecedario. Los caracteres que tendremos en cuenta
abc="abcdefghijklmnopqrstuvwxyz"
# Calculamos el número total de caracteres del texto a analizar
# Así no tendremos que calcularlo cada vez
textoLen=len(texto)

# Recorremos todos los caracteres del abecedario
for ch in abc:
 # Calculamos el porcentaje de aparición del caracter procesado
 # Lo redondeamos a 2 decimales para que tenga mejor presentación
 porc=round(100.0*count_char(texto,ch)/textoLen,2)
 # Si el porcentaje es superior a cero (si aparece este caracter)
 if porc>0:
  # Lo mostramos
  print("{} - {}%".format(ch,porc))

