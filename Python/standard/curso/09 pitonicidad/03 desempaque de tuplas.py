#Autor: Ramón Junquera
#Fecha: 20221223

# Pitonicidad. Desempaque de tuplas

# Podemos asignar los elementos de una tupla directamente a una colección
# de variables
miTupla=(1,2,3)
a,b,c=miTupla
print(a)
print(b)
print(c)

# Gracias a esta posibilidad se utiliza mucho para intercambiar valores
# entre variables
a=1
b=2
a,b=(b,a)
print(a)
print(b)

# Tambien funciona con listas
a,b=[1,2]
a,b=[b,a]
print(a)
print(b)

# Incluso no es necesario ni utilizar tuplas ni listas
a,b=1,2
a,b=b,a
print(a)
print(b)

# Siempre estamos asignando valores cuando tenemos el mismo número de
# elementos a ambos lados del igual.
# Si no es así. Si tenemos más valores que variables, deberíamos elegir
# una de las variables y ponerlo de prefijo un *
# Esto significa que esta variable tomará todos los valores que sobren
a,b,*c,d=(1,2,3,4,5,6,7,8,9)
print(a) # 1
print(b) # 2
print(c) # [3, 4, 5, 6, 7, 8]
print(d) # 9

# El mismo ejemplo que antes, pero utilizando rangos
a,b,*c,d=range(1,10)
print(a) # 1
print(b) # 2
print(c) # [3, 4, 5, 6, 7, 8]
print(d) # 9
