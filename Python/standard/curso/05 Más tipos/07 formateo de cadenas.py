#Tema: Formateo de cadenas
#Fecha: 20230110
#Autor: Ramón Junquera

# El objeto cadena tiene un método llamado format que facilita la
# inserción de variables dentro de un texto
# Cada par de llaves se sustituirá por el correspondiente
# parámetro en orden

# Ejemplo con un parámetro
msg="Frecuencia = {} Hz".format(60)
print(msg) #Resultado: Frecuencia = 60 Hz

# Ejemplo con dos parámetros
msg="Rectángulo: base = {}cm, altura={}cm".format(26,43)
print(msg) #Resultado: Rectángulo: base = 26cm, altura=43cm

# Si en el interior de las llaves escribimos un número entero
# se sustituirá por el parámetro de esa posición
msg="Rectángulo: base = {1}cm, altura={0}cm".format(26,43)
print(msg) #Resultado: Rectángulo: base = 43cm, altura=26cm

# Podríamos mostrar el mismo parámetro varias veces
msg="Cuadrado: base = {0}, altura = {0}, color = {1}".format(13,"verde")
print(msg) #Resultado: Cuadrado: base = 13, altura = 13, color = verde

# Ejemplo de repetición de parámetros con cadenas
print("{0}{1}{0}".format("abra","cad")) #Resultado: abracadabra

# Podríamos utilizar una tupla para guardar coordenadas
coor=(11,22,33)
msg="Coordenadas: x={},y={},z={}".format(coor[0],coor[1],coor[2])
print(msg) #Resultado: Coordenadas: x=11,y=22,z=33

# También podemos utilizar variables en el interior de las llaves
# pero tendrán que ser declaradas dentro del format
msg="Coordenadas: x={x},y={y},z={z}".format(z=33,x=11,y=22)
print(msg) #Resultado: Coordenadas: x=11,y=22,z=33

# Podemos combinar la los índices de parámetros con su nomenclatura
nombre="Pepe"
msg="Coordenadas: x={0},y={y},z={z}".format(nombre,z=33,y=22)
print(msg) #Resultado: Coordenadas: x=Pepe,y=22,z=33
