#Autor: Ramón Junquera
#Fecha: 20221219

# En Python, las variables no tienen tipo, ni necesitan ser
# definidas previamente
x=7
print(x) #Resultado: 7
print(x*2) #Resultado: 14
# También admiten valores de cadena
palabra='hola'
print(palabra*3) #Resultado: holaholahola
# El tipo de la variable sólo depende de su contenido
y=5.0
print(y*2) #Resultado: 10.0
y="Hola"
print(y*2) #Resultado: HolaHola

# Restricciones de nombres de variables
# Sólo se pueden utilizar letras (mayúsculas o minúsculas), números
# y guiones bajos. No se permiten los espacios.
# Una variable no puede comenzar por un número
mi_variable_favorita='hola'
mi_variable_favorita=mi_variable_favorita+' adios'
print(mi_variable_favorita) #Resultado: Hola adios

# Las variables diferencian entre mayúsculas y minúsculas
# Ejemplo: xy4 es distinto que xY4
xy4=4
xY4=5
print(xy4) #Resultado: 4
print(xY4) #Resultado: 5

# Una variable puede ser eliminada y liberada su memoria con
# la instrucción del
x='hola'
del x
# Ejemplo de asignación y eliminación
x=2
y=3
del x
y=4
x=5
print(x*y) #Resultado: 20

# Se pueden asignar distintos valores a varias variables en la misma línea
a,b,c=1,'dos',3.0
print(a) #Resultado: 1
print(b) #Resultado: dos
print(c) #Resultado: 3.0

#Se puede asignar el mismo valor a varias variables simultáneamente
a=b=c=8
print(a) #Resultado: 8
print(b) #Resultado: 8
print(c) #Resultado: 8
