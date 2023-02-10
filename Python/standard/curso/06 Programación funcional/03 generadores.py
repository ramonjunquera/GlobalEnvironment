#Autor: Ramón Junquera
#Fecha: 20221221

# Generadores
# Son objetos iterables, como las listas o las tuplas.
# La diferencia es que sólamente generan el elemento que se
# utiliza en ese momento. No genera todos los elementos y los 
# memoriza como en listas o tuplas. Por lo tanto, no es tan rápido,
# pero requere muchísima menos memoria para mantenerlo.

# Por supuesto un generador necesita una regla para poder crear
# cada uno de sus elementos
# Al no tener generados todos sus elementos no se pueden indexar

# Se crean igual que una función estándar, excepto que en vez de
# devolver un valor con return, lo hacen con yield
# Cada vez que se llama nos entrega el siguiente elemento
# Ejemplo. Generador de números pares desde el 0 al 8
def pares():
 i=0
 while i<=4:
  yield i*2
  i+=1
# Una vez definido el generador, lo llamamos desde un bucle for  
for i in pares():
 print(i)
# La sentencia yield lo que hace es devolver el valor que
# corresponde al elemento y detener la ejecución de la función
# hasta que se vuelva a llamar, que continuará donde lo había 
# dejado

# Puesto que un generador no almacena sus resultados sino que los
# va generando según se le vayan solicitando, se podría crear uno
# que genere infinitos elementos
def generadorInfinito():
 i=0
 while True:
  yield i
  i+=1
print("Generador infinito")
#for i in generadorInfinito():
# print(i) #No lo activamos para que pueda continuar con el resto de ejemplos


# Un generador finito se puede convertir en una lista con el 
# comando list
def pares():
 i=0
 while i<=4:
  yield i*2
  i+=1
miLista=list(pares())
print(miLista) #Resultado: [0, 2, 4, 6, 8]

# Un generador puede tener parámetros
# Ejemplo. Generador de números pares desde el 0 hasta un
# máximo definido por el parámetro
def pares(m):
 i=0
 while i*2<m:
  yield i*2
  i+=1
for i in pares(11):
 print(i)

# Tampoco hay problema en convertir un generador finito con
# parámetros en una lista
def pares(m):
 i=0
 while i*2<m:
  yield i*2
  i+=1
miLista=list(pares(11))
print(miLista) #Resultado: [0, 2, 4, 6, 8, 10]
