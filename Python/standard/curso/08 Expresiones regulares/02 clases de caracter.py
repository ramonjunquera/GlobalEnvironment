#Autor: Ramón Junquera
#Fecha: 20221221

# Expresiones regulares. Clases de carácter
# Permiten hacer coincidir con un sólo caracter con alguno de 
# los definidos en un grupo
# Los grupos de caracteres posibles se guardan entre corchetes
import re
def check_search(subcadena,cadena):
 if re.search(subcadena,cadena):
  print("search: {} contiene {}".format(cadena,subcadena))
 else:
  print("search: {} no contiene {}".format(cadena,subcadena))
check_search("[aeiou]","rosa")
check_search("[aeiou]","m23 + 7b = a + 25z")
check_search("[aeiou]","my rhythm")

# Otro ejemplo en el que se solicitan dos vocales seguidas
import re
def check_search(subcadena,cadena):
 if re.search(subcadena,cadena):
  print("search: {} contiene {}".format(cadena,subcadena))
 else:
  print("search: {} no contiene {}".format(cadena,subcadena))
check_search("[aeiou][aeiou]","flor rosa")
check_search("[aeiou][aeiou]","agua de mar")
check_search("[aeiou][aeiou]","antes de ayer")

# Ejemplo: Cualquier frase que acabe en s o vocal
import re
def check_search(subcadena,cadena):
 if re.search(subcadena,cadena):
  print("search: {} contiene {}".format(cadena,subcadena))
 else:
  print("search: {} no contiene {}".format(cadena,subcadena))
check_search("[saeiou]$","flor rosa")
check_search("[saeiou]$","agua de mar")
check_search("[saeiou]$","fin de mes")

# En las clases de caracteres se permiten rangos
# Ejemplo. Cualquier frase que tenga uba letra mayúscula
import re
def check_search(subcadena,cadena):
 if re.search(subcadena,cadena):
  print("search: {} contiene {}".format(cadena,subcadena))
 else:
  print("search: {} no contiene {}".format(cadena,subcadena))
check_search("[A-Z]","flor rosa")
check_search("[A-Z]","Flor Rosa")
check_search("[A-Z]","A las tres y media")

# Ejemplo: cualquier frase que contenga una letra minúscula,
# seguida de una mayúscula y de un número
import re
def check_search(subcadena,cadena):
 if re.search(subcadena,cadena):
  print("search: {} contiene {}".format(cadena,subcadena))
 else:
  print("search: {} no contiene {}".format(cadena,subcadena))
check_search("[a-z][A-Z][0-9]","Ana34")
check_search("[a-z][A-Z][0-9]","anA34")
check_search("[a-z][A-Z][0-9]","MANZANA34")
check_search("[a-z][A-Z][0-9]","mAnzAnAs y pErA5")

# Una clase puede tener más de un rango
# Ejemplo: Que comience por una letra. Puede ser mayúscula o minúscula
import re
def check_search(subcadena,cadena):
 if re.search(subcadena,cadena):
  print("search: {} contiene {}".format(cadena,subcadena))
 else:
  print("search: {} no contiene {}".format(cadena,subcadena))
check_search("^[a-zA-Z]","Ana34")
check_search("^[a-zA-Z]","a mi me gusta")
check_search("^[a-zA-Z]"," a mi no me gusta")
check_search("^[a-zA-Z]","4 a la vez")

# Ejemplo. Debe tener una número entre 10 y 59 separado por espacios
# por delante y por detrás
import re
def check_search(subcadena,cadena):
 if re.search(subcadena,cadena):
  print("search: {} contiene {}".format(cadena,subcadena))
 else:
  print("search: {} no contiene {}".format(cadena,subcadena))
check_search(" [1-5][0-9] ","33")
check_search(" [1-5][0-9] ","Son 33 elementos")
check_search(" [1-5][0-9] ","Son 72 elementos")
check_search(" [1-5][0-9] ","Son 33. Casi 34")

# Si incluimos el metacaracter ^ al inicio de una clase, la niega
# Servirá cualquier caracter que no esté en esa clase
# Ejemplo: Cualquier frase que no comience con mayúsculas
import re
def check_search(subcadena,cadena):
 if re.search(subcadena,cadena):
  print("search: {} contiene {}".format(cadena,subcadena))
 else:
  print("search: {} no contiene {}".format(cadena,subcadena))
check_search("^[^A-Z]","33")
check_search("^[^A-Z]"," Me gusta")
check_search("^[^A-Z]","Me gusta")
check_search("^[^A-Z]","me gusta")

# Dentro de una clase, el metacaracter ^ sólo tiene significado 
# al principio
# Ejemplo. Cualquier frase que contenga a o ^
import re
def check_search(subcadena,cadena):
 if re.search(subcadena,cadena):
  print("search: {} contiene {}".format(cadena,subcadena))
 else:
  print("search: {} no contiene {}".format(cadena,subcadena))
check_search("[a^]","Es mi casa")
check_search("[a^]","Es mi moto")
check_search("[a^]","3*3 = 3^2")
check_search("[a^]","3*3 = 3 al cuadrado")
check_search("[a^]","3*3 = 3 dos veces")
