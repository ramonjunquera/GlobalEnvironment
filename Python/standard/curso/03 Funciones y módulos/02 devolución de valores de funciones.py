#Autor: Ramón Junquera
#Fecha: 20221219

# Aunque no es obligatorio, las funciones pueden devolver un valor
# con el comando return

# Función que calcula el doble del primer parámetro
# Si el parámetro tiene valor numérico devolverá el doble
# Si es una cadena, devolverá la cadena repetida y concatenada 2 veces
def doble(a):
 return a*2
# Llamamos a la función
print(doble(7)) #Resultado: 14
print(doble("adios")) #Resultado: adiosadios

# Devuelve el máximo de los dos parámetros
def maximo(a,b):
 if a>b:
  return a
 else:
  return b
# Llamamos a la función
print(maximo(5,8)) #Resultado: 8
print(maximo("Zacarías","Asier")) #Resultado: Zacarías

# Devuelve la cadena más corta
def masCorta(c1,c2):
 if len(c1)<len(c2):
  return c1
 else:
  return c2
# Llamamos a la función
print(masCorta("Pedro","Juan")) #Resultado: Juan
print(masCorta("yo","vosotros")) #Resultado: yo

# Una vez ejecutado el comando return, finaliza la función
# No se ejecutará más código a continuación
# Reescribimos el ejemplo anterior sabiendo esto
def masCorta(c1,c2):
 if len(c1)<len(c2): return c1
 return c2
# Llamamos a la función
print(masCorta("Pedro","Juan")) #Resultado: Juan
print(masCorta("yo","vosotros")) #Resultado: yo
