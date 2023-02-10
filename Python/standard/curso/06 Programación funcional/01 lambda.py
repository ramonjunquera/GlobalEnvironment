#Autor: Ramón Junquera
#Fecha: 20221221

# Lambdas. Funciones anónimas

# Hasta ahora cuando queríamos utilizar una función debíamos
# definirla previamente
# Por ejemplo
def doble(x):
 return 2*x
print(doble(3))

# Si la función no será reutilizada en el futuro y sólo se usa
# en un punto único, se puede definir directamente con una 
# función lambda
print((lambda x:2*x)(3))

# El nombre de la función debe ser sustituido por el comando
# lambda seguido de los parámetros, seguido de dos puntos,
# seguido de la función
# Todo ello lo metemos entre parénteris y a continuación le pasamos
# los valores que se asignarán a los parámetros como cualquier
# función normal
# Ejemplo de función de dos parámetros. Función suma
print((lambda s1,s2:s1+s2)(2,3))

# Podríamos crear una función con lambda y asignarla a un objeto
# Sería lo mismo que definirla con def
suma=lambda s1,s2:s1+s2
print(suma(2,3))
# No es el uso habitual. Las funciones lambda se usa cuando no es
# necesaria su reutilización
