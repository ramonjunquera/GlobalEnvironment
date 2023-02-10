#Autor: Ramón Junquera
#Fecha: 20221221

# Expresiones regulares. Secuencias especiales

# Las secuencias especiales son las que comienzan por \

# Secuencia \d
# Representa un dígito. Es lo mismo que [0-9]
import re
def check_search(subcadena,cadena):
 if re.search(subcadena,cadena):
  print("search: {} contiene {}".format(cadena,subcadena))
 else:
  print("search: {} no contiene {}".format(cadena,subcadena))
check_search("\d(abc|def)","2334abc") #True
check_search("\d(abc|def)","233def") #True
check_search("\d(abc|def)","abcdef") #False
check_search("\d(abc|def)","abc0def") #True

# Secuencia \s
# Representa un espacio
import re
def check_search(subcadena,cadena):
 if re.search(subcadena,cadena):
  print("search: {} contiene {}".format(cadena,subcadena))
 else:
  print("search: {} no contiene {}".format(cadena,subcadena))
check_search("\d\shola","3 hola") #True
check_search("\d\shola","3-hola") #False
check_search("\d\shola","A hola") #False
check_search("\d\shola","3 holo") #False

# Secuencia \w
# Representa una letra mayúscula, minúscula o un número.
# Igual que [a-zA-Z0-9]
import re
def check_search(subcadena,cadena):
 if re.search(subcadena,cadena):
  print("search: {} contiene {}".format(cadena,subcadena))
 else:
  print("search: {} no contiene {}".format(cadena,subcadena))
check_search("--\w--","--3--") #True
check_search("--\w--","--a--") #True
check_search("--\w--","--B--") #True
check_search("--\w--","--ó--") #True
check_search("--\w--","--?--") #False
check_search("--\w--","-----") #False
check_search("--\w--","--;--") #False

# Secuencia \D
# Representa un carácter que no es un dígito. Es lo mismo que [^0-9]
import re
def check_search(subcadena,cadena):
 if re.search(subcadena,cadena):
  print("search: {} contiene {}".format(cadena,subcadena))
 else:
  print("search: {} no contiene {}".format(cadena,subcadena))
check_search("--\D--","--3--") #False
check_search("--\D--","--a--") #True
check_search("--\D--","--B--") #True
check_search("--\D--","--ó--") #True
check_search("--\D--","--?--") #True
check_search("--\D--","-----") #True
check_search("--\D--","--;--") #True

# Secuencia \S
# Representa un carácter que no es un espacio Es lo mismo que "^ "
import re
def check_search(subcadena,cadena):
 if re.search(subcadena,cadena):
  print("search: {} contiene {}".format(cadena,subcadena))
 else:
  print("search: {} no contiene {}".format(cadena,subcadena))
check_search("--\S--","--3--") #True
check_search("--\S--","--a--") #True
check_search("--\S--","--B--") #True
check_search("--\S--","--ó--") #True
check_search("--\S--","--?--") #True
check_search("--\S--","-----") #True
check_search("--\S--","-- --") #False

# Secuencia \W
# Representa un carácter que sea distinto a una letra mayúscula,
# minúscula o dígito
# Es lo mismo que ^[a-zA-Z0-9]
import re
def check_search(subcadena,cadena):
 if re.search(subcadena,cadena):
  print("search: {} contiene {}".format(cadena,subcadena))
 else:
  print("search: {} no contiene {}".format(cadena,subcadena))
check_search("--\W--","--3--") #False
check_search("--\W--","--a--") #False
check_search("--\W--","--B--") #False
check_search("--\W--","--ó--") #False
check_search("--\W--","--?--") #True
check_search("--\W--","-----") #True
check_search("--\W--","-- --") #True
check_search("--\W--","--;--") #True

# Ejemplo. Cadena que tenga uno o varios dígitos seguidos de uno
# o varias letras mayúsculas
import re
def check_search(subcadena,cadena):
 if re.search(subcadena,cadena):
  print("search: {} contiene {}".format(cadena,subcadena))
 else:
  print("search: {} no contiene {}".format(cadena,subcadena))
check_search("\d+[A-Z]+","34a") #False
check_search("\d+[A-Z]+","56A") #True
check_search("\d+[A-Z]+","0Abc") #True
check_search("\d+[A-Z]+","0abC") #False
check_search("\d+[A-Z]+","2ABC 45a") #True
check_search("\d+[A-Z]+","3-123ABC") #True
