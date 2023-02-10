#Autor: Ramón Junquera
#Fecha: 20221219

# La prioridad de operadores cuando se encuentran al mismo nivel
# es la siguiente
# **
# * / % //
# + -
# >> << (desplazamiento de bits)
# & (and bit a bit)
# ^ | (xor y or bit a bit)
# <= < > >= (operadores de comparación)
# <> == != (operadores de igualdad)
# = &= /= //= -= += *= **= (operadores de asignación)
# not or and (operadores lógicos)

# Ejemplo
if 1+1*3==6:
 print('si')
else:
 print('no') #Opción seleccionada
# Es lo mismo que:
if (1+(1*3))==6:
 print('si')
else:
 print('no') #Opción seleccionada

# Ejemplo
x=4
y=2
if not 1+1==y or x==4 and 7==8:
 print('si')
elif x>y:
 print('no') #Opción seleccionada
# Es lo mismo que:
x=4
y=2
if ((not ((1+1)==y)) or x==4) and (7==8):
 print('si')
elif x>y:
 print('no') #Opción seleccionada