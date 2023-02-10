#Autor: Ramón Junquera
#Fecha: 20221229

# Expresiones regulares. Grupos
# Los caracteres se pueden agrupar con paréntesis
# En ejercicios anteriores hemos utilizado algún ejemplo con
# grupos:
# "A(ma)+" = Una a mayúscula seguida por la sílaba ma repetida
# una o más veces

# EJemplo. Una cadena que contenga al menos una repetición de la
# secuencia: una letra no vocal, una vocal y una o más no vocales
import re
def check_search(subcadena,cadena):
 if re.search(subcadena,cadena):
  print("search: {} contiene {}".format(cadena,subcadena))
 else:
  print("search: {} no contiene {}".format(cadena,subcadena))
check_search("([^aeiou][aeiou][^aeiou])+","babcec") #True
check_search("([^aeiou][aeiou][^aeiou])+","Aab") #True
check_search("([^aeiou][aeiou][^aeiou])+","aabee") #False

# El método search devuelve un objeto match que indica las coincidencias
# que ha podido localizar
import re
def show_search(subcadena,cadena):
 print("search: {} contiene {}".format(cadena,subcadena))
 coincidencias=re.search(subcadena,cadena)
 num_coincidencias=int(str(coincidencias)[11:-1])
 for i in range(num_coincidencias):
  print("{} : {}".format(i,coincidencias.group(i)))  
show_search("([^aeiou][aeiou][^aeiou])+","babcec")
# El objeto match de Micropython sólo contiene la función group()
# No hay ningún método para saber el número de grupos/coincidencias
# Afortunadamente cuando consultamos un objeto match obtenemos como respuesta:
#  <match num=2>
# Si lo convertimos en cadena podemos extraer el valor del contador, porque sabemos
# que se encuentra a partir de la posición 11 y debemos excluir el último carácter
# En este caso devuelve 2: babcec y cec

# En los grupos podemos incluir el metacaracter | para tomar
# la expresión de antes o de después del metacaracter
import re
def check_search(subcadena,cadena):
 if re.search(subcadena,cadena):
  print("search: {} contiene {}".format(cadena,subcadena))
 else:
  print("search: {} no contiene {}".format(cadena,subcadena))
check_search("mant(i|equi)lla","mantilla") #True
check_search("mant(i|equi)lla","mantequilla") #True
check_search("mant(i|equi)lla","mantoquilla") #False

