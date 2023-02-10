#Autor: Ramón Junquera
#Fecha: 20221221

# Expresiones regulares. Metacaracteres sencillos.

# Expresiones regulares es una potente herramienta de gestión de
# cadenas. Permite mucha flexibilidad para la identificación
# de patrones o substitución de substrings.
# No es una característica propia de Python, existe en otros lenguajes.

# En Python las clases para la gestión de las expresiones
# regulares están en el módulo re.

# Método match
# Permite comprobar si una cadena comienza por una subcadena
import re
cadena="holaholahola"
subcadena1="hola"
subcadena2="laho"
if re.match(subcadena1,cadena):
 print("{} comienza con {}".format(cadena,subcadena1))
else:
 print("{} no comienza con {}".format(cadena,subcadena1))
if re.match(subcadena2,cadena):
 print("{} comienza con {}".format(cadena,subcadena2))
else:
 print("{} no comienza con {}".format(cadena,subcadena2))

# Método search
# Comprueba si una subcadena forma parte de una cadena
# La subcadena puede estar en cualquier posición
import re
def check_search(subcadena,cadena):
 if re.search(subcadena,cadena):
  print("{} incluye {}".format(cadena,subcadena))
 else:
  print("{} no incluye {}".format(cadena,subcadena))
miCadena="holaholahola"
check_search("hola",miCadena)
check_search("laho",miCadena)
check_search("lala",miCadena)

# Las expresiones que se pueden incluir en estos métodos mediante
# metacaracteres son las que hacen potente el sistema de gestión
# de cadenas.

# Metacaracter .
# Es la sustitución de cualquier carácter distinto a un salto
# de línea, pero sólo uno
import re
def check_search(subcadena,cadena):
 if re.search(subcadena,cadena):
  print("{} incluye {}".format(cadena,subcadena))
 else:
  print("{} no incluye {}".format(cadena,subcadena))
check_search("t.ngo","yo tengo un coche")
check_search("t.ngo","me gusta bailar tango")
check_search("t.ngo","mi coche es un twingo")

# Otro ejemplo para mostar que se pueden utilizar metacarateres
# seguidos
import re
def check_search(subcadena,cadena):
 if re.search(subcadena,cadena):
  print("{} incluye {}".format(cadena,subcadena))
 else:
  print("{} no incluye {}".format(cadena,subcadena))
check_search("a..a","Tienes una mensaje de Marta")
check_search("a..a","ama me llama")
check_search("a..a","deja de mirar a la pared")
check_search("a..a","misil ah3a")
check_search("a..a","a+3a=4a")

# Metacaracter ^
# Indica el inicio de la cadena, comienza con
import re
def check_search(subcadena,cadena):
 if re.search(subcadena,cadena):
  print("{} incluye {}".format(cadena,subcadena))
 else:
  print("{} no incluye {}".format(cadena,subcadena))
check_search("^ama","camarero")
check_search("^ama","amapola")
check_search("^ama","vete a tu cama")
# Cuando utilizamos match sabemos que siempre se comienza desde
# el principio. No sería necesario utilizar ^

# Metacaracter $
# Indica el final de la cadena, finaliza con
import re
def check_match(subcadena,cadena):
 if re.search(subcadena,cadena):
  print("{} incluye {}".format(cadena,subcadena))
 else:
  print("{} no incluye {}".format(cadena,subcadena))
check_match(".os.$","Rosa")
check_match(".os.$","Rosas")
check_match(".os.$","los")
check_match(".os.$","cose")
check_match(".os.$"," oso")
check_match(".os.$","oso")
check_match(".os.$","acoso")

# Un ejempl más para comprobar las diferencias entre el método
# match y search
import re
def check_match(subcadena,cadena):
 if re.match(subcadena,cadena):
  print("match: {} comienza con {}".format(cadena,subcadena))
 else:
  print("match: {} no comienza con {}".format(cadena,subcadena))
def check_search(subcadena,cadena):
 if re.search(subcadena,cadena):
  print("search: {} incluye {}".format(cadena,subcadena))
 else:
  print("search: {} no incluye {}".format(cadena,subcadena))
check_match(".os.$","acoso")
check_search(".os.$","acoso")
