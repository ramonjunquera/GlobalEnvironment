#Tema: Funciones de cadenas
#Fecha: 20230111
#Autor: Ramón Junquera

# El método capitalize pone en mayúscula la primera de las letras de la cadena
print("pedro".capitalize()) #Resultado: Pedro
print("pedro casas".capitalize()) #Resultado: Pedro casas (sólo afecta a la primera letra de la cadena, no de las palabras)
print(" hola".capitalize()) #Resultado:  hola (el espacio en mayúsculas es también un espacio)
print("123 hola") #Resultado: 123 hola (los números en mayúscula son ellos mismos)

# El método join insertará la cadena como separador de los valores
# pasados como parámetros
# Resultado: uno <-> dos <-> tres
msg=" <-> ".join(["uno","dos","tres"]) #Resultado: uno <-> dos <-> tres
print(msg)

# El método partition divide una cadena mediante un separador
# Devuelve una tupla con 3 elementos: el prefijo, el separador y el sufijo
# Si el separador aparece varias veces, sólo se tiene en cuenta la primera
print("https://www.google.es".partition("www.")) #Resultado: ('https://', 'www.', 'google.es')
print("www.google.es".partition(".")) #Resultado: ('www', '.', 'google.es')
print("www.google.es".partition("=")) #Resultado: ('www.google.es', '', '')
#Si no se encuentra el separador, la cadena original será el prefijo. Y no habrá si separador ni sufijo. 

# El método replace reemplaza una subcadena por otra
msg="Me gusta el vino".replace("vino","agua") #Resultado: Me gusta el agua
print(msg)

# Si la subcadena no se encuentra no se produce una excepción
# Simplemente no hace nada
msg="Me gusta el vino".replace("whisky","agua") #Resultado: Me gusta el vino
print(msg)

# El método startswith devuelve un booleano. True si la cadena
# comienza por la palabra indicada en el parámetro
if "Esto es una prueba".startswith("Est"):
  print("Comienza por Est") #*
else:
  print("No comienza por Est")

# Es importante tener en cuenta que se distinguen mayúsculas
# y minúsculas
if "Esto es una prueba".startswith("est"):
  print("Comienza por est")
else:
  print("No comienza por est") #*

# El método endswith funciona igual pero pero se comrpueba el final
if "Esto es una prueba".endswith("eba"):
  print("Termina en eba") #*
else:
  print("No termina en eba")

# El método upper() pasa todo el texto a mayúsculas
msg="Esto es una prueba".upper()
print(msg) #Resultado: ESTO ES UNA PRUEBA

# El método lower() pasa todo el texto a minúsculas
msg="Esto Es Una Prueba".lower()
print(msg) #Resultado: esto es una prueba

# El método swapcase() intercambia todas las letras de la cadena entre mayúsculas y minúsculas
msg="Esto Es Una Prueba".swapcase()
print(msg) #Resultado: eSTO eS uNA pRUEBA

# El método title() pone en mayúscula la primera de las letras de cada palabra de la cadena
msg="esto Es la prueba 23".title()
print(msg) #Resultado: Esto Es La Prueba 23

# El método split() corta una cadena por la subcadena indicada en el
# parámetro y devuelve una lista con sus elementos
print("uno,dos,tres".split(",")) #Resultado: ['uno', 'dos', 'tres']
print("uno,dos,tres".split(";")) #Resultado: ['uno,dos,tres']
#Nota. Cuando no se encuentra el separador, la tupla resultante tendrá como único elemento la cadena completa.

# El método splitlines() devuelve una tupla con cada una de las líneas de una cadena
# Lo que hace es tomar como separador el retorno de carro
msg="""L1
L2
L3
"""
print(msg.splitlines()) #Resultado: ['L1', 'L2', 'L3']
#También podemos definir la cadena insertando los retornos de carro con su correspondiente código
msg="lin1\nlin2\nlin3"
print(msg.splitlines()) #Resultado: ['lin1', 'lin2', 'lin3']

# El método center() genera una cadena de cierta longitud, centrando otra y con cierto carácter de relleno
# El caracter de rellno sólo puede ser uno. No una cadena. Sino dará una excepción
print("mi título".center(50,"="))   #Resultado: ====================mi título=====================
print(" mi título ".center(50,"=")) #Resultado: =================== mi título ====================
print("Este es mi título con 35 caracteres".center(30,"=")) #Resultado: Este es mi título con 35 caracteres
#Cuando la cadena es mayor que la longitud indicada, se devuelve la cadena original

# El método ljust() genera una cadena de cierta longitud, justificando a la izquierda otra y con cierto carácter de relleno
# El caracter de rellno sólo puede ser uno. No una cadena. Sino dará una excepción
print("mi título".ljust(50,"=")) #Resultado: mi título=========================================

# El método rjust() genera una cadena de cierta longitud, justificando a la derecha otra y con cierto carácter de relleno
# El caracter de rellno sólo puede ser uno. No una cadena. Sino dará una excepción
print("mi título".rjust(50,"=")) #Resultado: =========================================mi título

# El método zfill() genera una cadena de cierta longitud, rellenando con ceros a la izquierda
print("12345".zfill(10)) #Resultado: 0000012345
print(str(12345).zfill(10)) #Resultado: 0000012345
print("carlos".zfill(10)) #Resultado: 0000carlos
print("carlos".rjust(10,"0")) #Resultado: 0000carlos (mismo resultado que el ejemplo anterior)
print("12345678890".zfill(5)) #Resultado: 1234567890
#Cuando la cadena es mayor que la longitud indicada, se devuelve la cadena original

# El método count devuelve el número de veces que se detecta una cadena en otra
print("bienvenido vecino".count("ve")) #Resultado: 2

# El método find() devuelve la posición de la primera vez que se encuentra una cadena en otra
# 01234567890123456
# bienvenido vecino
print("bienvenido vecino".find("ve")) #Resultado: 4 (recordemos que la primera posición tiene el índice 0)
print("bienvenido vecino".find("pe")) #Resultado: -1 (cuando no se encuentra la subcadena se devuelve -1)
# El método admite un segundo parámetro opcional con la posición inicial de búsqueda
print("bienvenido vecino".find("ve",5)) #Resultado: 11
# El método admite un tercer parámetro opcional con la posición final de búsqueda
print("bienvenido vecino".find("ve",5,10)) #Resultado: -1 (no encontrado entre la posición 5 y 10)

# El método isalnum() indica si todos los caracteres que forman la cadena son letras (mayúsculas o 
# minúsculas) o dígitos
print("prueba".isalnum()) #Resultado: True
print("123".isalnum()) #Resultado: True
print("prueba25".isalnum()) #Resultado: True
print("Prueba25".isalnum()) #Resultado: True (las mayúsculas también son letras)
print("prueba25.3".isalnum()) #Resultado: False (porque el punto no es ni letra ni dígito)
print("prueba25 nombre".isalnum()) #Resultado: False (porque el espacio no es ni letra ni dígito)

# El método isalpha() indica si todos los caracteres que forman la cadena son letras (mayúsculas o 
# minúsculas)
print("prueba".isalpha()) #Resultado: True
print("Prueba".isalpha()) #Resultado: True (las mayúsculas también son letras)
print("123".isalpha()) #Resultado: False
print("prueba25".isalpha()) #Resultado: False
print("prueba.".isalpha()) #Resultado: False (porque el punto no es ni letra ni dígito)
print("prueba nombre".isalpha()) #Resultado: False (porque el espacio no es ni letra ni dígito)

# El método isdigit() indica si todos los caracteres que forman la cadena son dígitos
print("123".isdigit()) #Resultado: True
print("prueba2".isdigit()) #Resultado: False
print("0.0".isdigit()) #Resultado: False (porque el punto no es dígito)

# El método islower() indica si todos los caracteres que forman la cadena están en minúscula
print("prueba".islower()) #Resultado: True
print("prueba once".islower()) #Resultado: True (el espacio es igual en mayúsculas que en minúsculas)
print("prueba.once".islower()) #Resultado: True (en general los caracteres especiales no distinguen mayúsculas y minúsculas)
print("Prueba".islower()) #Resultado: False
print("123".islower()) #Resultado: False (los dígitos NO se pueden poner en minúsculas!!!)

# El método isupper() indica si todos los caracteres que forman la cadena están en mayúscula
print("PRUEBA".isupper()) #Resultado: True
print("PRUEBA DOCE".isupper()) #Resultado: True (el espacio es igual en mayúsculas que en minúsculas)
print("PRUEBA.DOCE".isupper()) #Resultado: True (en general los caracteres especiales no distinguen mayúsculas y minúsculas)
print("Prueba".isupper()) #Resultado: False
print("123".isupper()) #Resultado: False (los dígitos NO se pueden poner en mayúsculas!!!)

# El método isspace() indica si todos los caracteres que forman la cadena son espacios
print("    ".isspace()) #Resultado: True
print(" ".isspace()) #Resultado: True
print(" _ ".isspace()) #Resultado: False

# El método istitle() indica si las palabras que componen la cedena tienen formato de título (si su primer aletra está en mayúsculas)
print("Esto Es Una Prueba".istitle()) #Resultado: True
print("Esto Es Una   Prueba".istitle()) #Resultado: True
print("Esto Es La Prueba 32".istitle()) #Resultado: True
print("El Resultado Es 34.2".istitle()) #Resultado: True
print("Esto Es UNA Prueba".istitle()) #Resultado: False

# El método strip recorta espacios iniciales y finales de una cadena
print("  hola   ".strip()) #Resultado: hola
print("  hola   amigo  ".strip()) #Resultado: hola   amigo (no recorta los espacios internos)
# Se puede indicar una colección de caracteres que deben ser recortados
print("--hola---".strip("-")) #Resultado: hola
print("- -hola- --".strip("-")) #Resultado:  -hola- 
print("--hola-amigo---".strip("-")) #Resultado: hola-amigo
print("<>hola<><<<<".strip("<>")) #Resultado: hola

# El método lstrip() recorta espacios por la izquierda (inicio)
print("  casa   ".lstrip()) #Resultado: casa   
print("  casa   amigo  ".lstrip()) #Resultado: casa   amigo   (no recorta los espacios internos)
# Se puede indicar una colección de caracteres que deben ser recortados
print("--casa---".lstrip("-")) #Resultado: casa---
print("--casa-amigo---".lstrip("-")) #Resultado: casa-amigo---
print("<<>><><<<<>>casa<><<<<".lstrip("<>")) #Resultado: casa<><<<<

# El método rstrip() recorta espacios por la derecha (final)
print("  mesa   ".rstrip()) #Resultado:   mesa   
print("  mesa   amigo  ".rstrip()) #Resultado:   mesa   amigo (no recorta los espacios internos)
# Se puede indicar una colección de caracteres que deben ser recortados
print("--mesa---".rstrip("-")) #Resultado: --mesa
print("--mesa-amigo---".rstrip("-")) #Resultado: --mesa-amigo
print("<<><>mesa<><<<<".rstrip("<>")) #Resultado: <<><>mesa






