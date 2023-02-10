#Tema:cadenas (string)
#Fecha: 20230111
#Autor: Ramón Junquera

# Las cadenas siempre se delimitan por comillas o apóstrofes
print("hola")
print('hola')

# Incluir un apóstrofe o unas comillas en una cadena
# Si el carácter a incluir es distinto al delimitador, se puede
# incluir directamente
# Sino, tendrá que ser precedido de la contrabarra \
print('Tom"s home') #Resultado: Tom"s home
print("Tom\'s home") #Resultado: Tom's home
print("Tom's home") #Resultado: Tom's home
print("Tom\"s home") #Resultado: Tom"s home

# Saltos de línea
print('Hola\nAdios')

# También se puede asignar una cadena de varias líneas
# Si tenemos una cadena de varias líneas y no queremos utilizar el carácter de control
# \n, podemos comenzar por 3 comillas, escribir las líneas de la cadena y finalizar
# con otras 3 comillas
print("""
Hola
caracola
""")

"""
De la misma manera que se pueden escribir cadenas de varias líneas comenzando
y terminado con 3 comillas, se pueden utilizar para los comentarios.
Esta vez no tienen que estar precedidos por un #
"""

# Longitud de una cadena
print(len("Hola")) #Resultado: 4