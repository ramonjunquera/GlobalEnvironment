#Autor: Ramón Junquera
#Fecha: 20221219

# Condicionales
# Sintaxis: if + condición + :
# Si la condición en True, se ejecutarán todas las líneas que
# a continuación estén identadas con, al menos, un espacio
x=3
if x==5:
 print('x es cinco')
if x==3:
 print('x es tres')
if x<4:
 print('x es menor que cuatro')
# La identación de la condicional viene definida por la primera de
# sus líneas. El resto, si es que las hay, deben tener la misma
# identación que la primera
x=5
if x==5:
 print('hola')
 print('adios')
# También sería correcto el siguiente ejemplo
x=5
if x==5:
      print('hola')
      print('adios')
# Se pueden anidar
x=12
if x>5:
 print('mayor que 5')
 if x<22:
   print('menor que 22')
