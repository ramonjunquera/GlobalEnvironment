#Autor: Ramón Junquera
#Fecha: 20221223

# Pitonicidad. Sentencias else

# Si añadimos la sentencia else al final de un bucle for, su contenido
# será ejecutado cuando el bucle haya finalizado naturalmente. Si break
for i in range(10):
 if i == 60:
  print("bucle finalizado antes de terminar")
  break
else:
 print("bucle recorrido hasta el final")

# Lo mismo ocurre en una bucle while
i=0
while i < 10:
 if i == 60:
  print("bucle finalizado anter de terminar")
  break
 i+=1
else:
 print("bucle recorrido hasta el final")

# También pordemos añadir la sentecia else en una sentencia try/except
# Si no se producen errores y la secuencia finaliza naturalmente, se 
# tendrá en cuenta el código de la sección else
try:
 print(1/2)
except ZeroDivisionError:
 print("Error al dividir por cero")
else:
 print("try ejecutado sin errores")

# El caso contrario
try:
 print(1/0)
except ZeroDivisionError:
 print("Error al dividir por cero")
else:
 print("try ejecutado sin errores")

# También podemos incluir la sentencias finally
# La sección finally siempre se ejecuta al final, después de la sección
# else, si es que existe
try:
 print(1/2)
except ZeroDivisionError:
 print("Error al dividir por cero")
else:
 print("try ejecutado sin errores")
finally:
 print("sección finally")