#Autor: Ramón Junquera
#Fecha: 20230104

# Operador and
print(True and True) #Resultado: True
print(True and False) #Resultado: False
print(False and False) #Resultado: False
print(1==1 and 3>2) #Resultado: True
# Operador or
print(True or True) #Resultado: True
print(False or True) #Resultado: True
print(False or False) #Resultado: False
# Operador not
print(not True) #Resultado: False
print(not False) #Resultado: True
# Operador xor ^
print("xor FF",False ^ False) #Resultado: False
print("xor FT",False ^ True) #Resultado: True
print("xor TT",True ^ True) #Resultado: False

# Ejemplo
x=3
if not x==2:
 print('No es dos')
