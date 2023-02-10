#Tema: Operaciones con listas
#Fecha: 20221219
#Autor: Ramón Junquera

# Los valores de una lista pueden ser actualizados individualmente
miLista=[7,7,7,7,7]
miLista[2]=5
print(miLista) #Resultado: [7, 7, 5, 7, 7]
# Las listas pueden ser concatenadas como las cadenas
miLista=[1,2,3]
miLista+=[4,5,6]
print(miLista) #Resultado: [1, 2, 3, 4, 5, 6]
# Las lista pueden ser duplicadas como las cadenas
miLista=[1,2,3]
miLista=miLista*2
print(miLista) #Resultado: [1, 2, 3, 1, 2, 3]
# También admite operadores de asignación
miLista=[1,2,3]
miLista*=2
print(miLista) #Resultado: [1, 2, 3, 1, 2, 3]
# Se puede comprobar si existe un elemento en una lista
miLista=['uno','dos','tres']
if 'uno' in miLista:
  print('uno está en la lista') #*
if 'cuatro' in miLista:
  print('cuatro está en la lista')

# El mismo comando in permite comprobar la existencia de 
# substrings en una cadena
palabra='me gusta el chocolate'
if 'cola' in palabra:
  print('cola está en "' + palabra + '"')

# También se puede compobar si una lista no contiene un elemento
miLista=[1,2,3]
if 4 not in miLista:
  print('4 no está en la lista')
# También podría utilizarse la siguiente expresión:
if not 4 in miLista:
  print('4 no está en la lista')
