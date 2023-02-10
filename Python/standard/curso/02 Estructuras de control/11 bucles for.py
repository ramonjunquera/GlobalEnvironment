#Autor: Ramón Junquera
#Fecha: 20221219

# Los bucles for son como foreach en otros lenguajes
# El siguiente ejemplo recorre todos los elementos de una lista y los imprime
miLista=['uno','dos','tres']
for elemento in miLista:
  print(elemento)
# El siguiente ejemplo muestra el doble de cada elemento de la lista original
miLista=[1,2,3,4,5]
for elemento in miLista:
  print(elemento*2)

# Lo habitual es utilizar los bucles for combinados con los rangos para repetir
# un determminado número de veces un bucle
# El siguiente ejemplo imprime 5 veces la palabra hola
for i in range(5):
 print('hola')
# El siguiente ejemplo muestra los valores pares menores que 20
for i in range(0,20,2):
 print(i)
