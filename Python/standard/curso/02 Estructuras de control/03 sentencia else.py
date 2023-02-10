#Autor: Ramón Junquera
#Fecha: 20221219

# Las sentencias if se pueden completar con else
x=5
if x==4:
 print('Es cuatro')
else:
 print('No es cuatro')
# También se pueden anidar
x=5
if x==1:
 print('Es uno')
else:
 if x==2:
  print('Es dos')
 else:
  if x==3:
   print('Es tres')
  else:
   print('No es 1 ni 2 ni 3')
# Se pueden encadenar secuencias else if con el comando elif sin
# tener que aumentar la identación
x=5
if x==1:
 print('Es uno')
elif x==2:
 print('Es dos')
elif x==3:
 print('Es tres')
else:
 print('No es 1 ni 2 ni 3')