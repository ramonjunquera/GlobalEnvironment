#Autor: Ramón Junquera
#Fecha: 20221221

# Métodos mágicos. Contenedores

# Los métodos mágicos también pueden actuar como contenedores
# Este es el listado:
# __len__ len()
# __getitem__ para indexar
# __setitem__ para asignar valores indexados
# __delitem__ para borrar valores indexados
# __iter__ para iterar (como in en bucles)
# __contains__ para in
# __call__ para llamar  objetos como funciones
# __int__
# __str__

# Ejemplo de uso de __getitem__
# Crearemos una clase que mantendrá una lista
# Definimos el método mágico __getitem__ que será llamado
# cuando se solicite un elemento de la clase por su índice
# En ese caso, devolveremos el elemento previo y el solicitado
# concatenándolos.
# Si nos solicitan el primer elemento, como no tiene previo,
# entregaremos sólo él
class MiListaAcumulada:
 def __init__(self,miLista):
  self.miLista=miLista
 def __getitem__(self,index):
  if index==0: # Si nos piden el primer elemento...
    return self.miLista[0] # ...sólo devolvemos el primero
  # Si nos piden cualquier otro, devolveremos el previo y el 
  # solicitado concatenandolos
  return self.miLista[index-1]+self.miLista[index]
miLista=["a","b","c","d"] # Definimos una lista de cadenas
# Creamos una instancia de la clase con la lista de cadenas
miListaAcumulada=MiListaAcumulada(miLista)
# Recorremos todos los elementos de la instancia
for i in miListaAcumulada:
 # Mostramos cada uno de sus elementos
 print(i)
# El resultado es
# a
# ab
# bc
# cd
