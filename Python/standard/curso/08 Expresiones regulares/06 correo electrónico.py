#Autor: Ramón Junquera
#Fecha: 20221221

# Expresiones regulares. Correo electrónico

# Con lo que hemos aprendido acerca de metacaracteres, haremos un ejecicio
# típico: extraer la dirección de correo electrónico de un texto.
import re
cadena="Por favor contacte con pedro.picapiedra@picapiedras.com para más ayuda"
subcadena="(\w|\.|-)+@(\w|-)+\.(\w|\.|-)+"
grupos = re.search(subcadena,cadena)
print(grupos.group(0))
# Explicamos el significado de la subcadena:
# ( comienzo del primer grupo
# \w se admite cualquier letra mayúscula o minúscula o dígito
# | o tambien
# \. el caráctr punto
# | o también
# - el carácter menos
# ) fin del primer grupo
# + este primer grupo se debe repetir al menos una vez
# @ tras finalizar las repeticiones del primer grupo debe ir una @
# ( inicio del segundo grupo
# \w se admite cualquier letra mayúscula o minúscula o dígito
# | o tambien
# - el carácter menos
# ) fin del segundo grupo
# + este segundo grupo se debe repetir al menos una vez
# \. tras finalizar las repeticiones del segundo grupo debe ir un punto
# ( inicio del tercer grupo
# \w se admite cualquier letra mayúscula o minúscula o dígito
# | o tambien
# \. el caráctr punto
# | o también
# - el carácter menos
# ) fin del tercer grupo
# + este tercer grupo se debe repetir al menos una vez
