#Autor: Ramón Junquera
#Fecha: 20221221

# Expresiones regulares. Metacaracteres de repetición

# Metacaracter *
# Significa cero o más repeticiones de lo anterior
# Ejemplo. Que contenga una A mayúscula y cero o más repeticiones
# de la sílaba ma
import re
def check_search(subcadena,cadena):
 if re.search(subcadena,cadena):
  print("search: {} contiene {}".format(cadena,subcadena))
 else:
  print("search: {} no contiene {}".format(cadena,subcadena))
check_search("A(ma)*","Me llama Ama") #True
check_search("A(ma)*","Me llama Amama") #True
check_search("A(ma)*","A la cama!") #True
check_search("A(ma)*","Me voy a la cama") #False

# Metacaracter +
# Similar al *, excepto que se debe repetir al menos una vez
# Ejemplo. Que contenga una A mayúscula y al menos una repetición
# de la sílaba ma
import re
def check_search(subcadena,cadena):
 if re.search(subcadena,cadena):
  print("search: {} contiene {}".format(cadena,subcadena))
 else:
  print("search: {} no contiene {}".format(cadena,subcadena))
check_search("A(ma)+","Me llama Ama") #True
check_search("A(ma)+","Me llama Amama") #True
check_search("A(ma)+","A la cama!") #False
check_search("A(ma)+","Me voy a la cama") #False

# Metacaracter ?
# Significa cero o una repetición
# Ejemplo. Que contenga una A mayúscula y cero o una repetición
# de la sílaba ma
import re
def check_search(subcadena,cadena):
 if re.search(subcadena,cadena):
  print("search: {} contiene {}".format(cadena,subcadena))
 else:
  print("search: {} no contiene {}".format(cadena,subcadena))
check_search("A(ma)?","Me llama Ama") #True
check_search("A(ma)?","Me llama Amama") #True
check_search("A(ma)?","A la cama!") #True
check_search("A(ma)?","Me voy a la cama") #False

# Metacaracter {} llaves
# Indican que el número de repeticiones debe estar incluido en 
# un rango (un mínimo y un máximo)
import re
def check_search(subcadena,cadena):
 if re.search(subcadena,cadena):
  print("search: {} contiene {}".format(cadena,subcadena))
 else:
  print("search: {} no contiene {}".format(cadena,subcadena))
check_search("A(ma){0,1}","Me llama Ama") #True
check_search("A(ma){0,1}","Me llama Amama") #True
check_search("A(ma){0,1}","A la cama!") #True
check_search("A(ma){0,1}","Me voy a la cama") #False