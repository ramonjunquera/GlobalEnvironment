#Autor: Ramón Junquera
#Fecha: 20221220

# Funciones numéricas

# La función min() obtiene el mínimo valor de una tupla o lista
miTupla=(3,1,7,0,-5,-2,3,1)
print(min(miTupla))

# Se pueden mezclar valores del mismo tipo: enteros con flotantes, 
# pero no numéricos con cadenas porque no podría compararlos
miTupla=(3,1,7,0.5,-5,-2,3,1,7.3)
print(min(miTupla))

# No hay problema en comparar cadenas entre sí para obtener
# la primera en orden alfabético
miTupla=("uno","dos","tres")
print(min(miTupla))

# La función max() devuelve el máximo elemento
miLista=["uno","dos","tres"]
print(max(miLista))

# La función abs() devuelve el valor absoluto de un número
print(abs(47))
print(abs(-47))

# La función sum() devuelve la suma de una lista o tupla
# Los elementos deben ser todos numéricos. No se admiten cadenas
# Resultado: 50.0
miLista=[10,20,10,5.4,4.6]
print(sum(miLista))

# La funcion all() devuelve un booleano que es True si se cumple
# la condición del parámetro en todos los elementos de una
# lista o tupla
miLista=[11,22,33,44,55]
if all(i>5 for i in miLista):
  print("Todos son mayores que 5")
else:
  print("Alguno no es mayor que 5")

# También se puede aplicar a valores de cadena
# Pero no se pueden mezclar elementos numéricos y de cadena
# porque no sabe compararlos
miTupla=("amigo","bonito","cuerno","dinero")
palabra="zapato"
if all(i<palabra for i in miTupla):
  print("Todos son menores que " + palabra)
else:
  print("Alguno es mayor que " + palabra)

# La función any() devuelve un booleano que es True si se cumple
# la condición del parámetro en alguno de los elementos de una 
# lista o tupla
miLista=[11,22,33,44,55]
numero=66
if any(i>numero for i in miLista):
  print("Alguno es mayor que {}".format(numero))
else:
  print("Ninguno es mayor que {}".format(numero))

# Enumerate devuelve tantas tuplas como elementos
# Las tuplas son de dos elementos
# El primero es el índice y el segundo el valor
miLista=[11,22,33,44,55]
for v in enumerate(miLista):
  msg="Tupla de elemento: {}. Índice: {}. Valor: {}".format(v,v[0],v[1])
  print(msg)
