#Autor: Ramón Junquera
#Fecha: 20221221

# Métodos mágicos. Operaciones
# Son aquellos métodos que comienzan y terminan por doble guión
# bajo.
# En inglés se llaman dunders, de double underscores.
# Hasta ahora sólo hemos conocido el método __init__ como constructor
# Se utilizan para métodos con funcionalidades especiales que no
# se pueden representar con métodos regulares

# Método __add__
# Se llamará a este método cuando se pretenda utilizar el símbolo +
# para sumar dos clases
# En el siguiente ejemplo creamos la clase Vector, representando un 
# vector de dos dimensiones.
# El constructor almacena en variables de instancia las coordenadas
# horizontal y vertical del vector
# Añadimos el método show para que muestre las coordenadas del 
# vector.
# Y para finalizar la clase añadimos el método mágico __add__
# En este tipo de métodos están involucradas dos clases
# Por conveniencia llamamos self a la primera y other a la segunda
# Realmente podríamos ponerles cualquier nombre
# La suma de dos vectores da un nuevo vector con cuya coordenada
# horizontal es la suma de las coordenadas horizontales de los 
# vectores sumados. Lo mismo ocurre con la vertical.
# Después instanciamos dos vectores, los sumamos y guardamos el
# resultado en una tercera variable (instancia) de la que usamos
# el método show para mostrar sus coordenadas
class Vector:
 def __init__(self,x,y):
  self.x=x
  self.y=y
 def show(self):
  print("x={},y={}".format(self.x,self.y))
 def __add__(self,other):
  return Vector(self.x+other.x,self.y+other.y)
vector1=Vector(3,2)
vector2=Vector(4,6)
vectorResultado=vector1+vector2
vectorResultado.show()

# Hay más métodos mágicos cuyo funcionamiento es idéntico
# Lo único que cambia es el símbolo utilizado
# Este es el listado:
# __add__ +
# __sub__ -
# __mul__ *
# __truediv__ /
# __floordiv__ //
# __mod__ %
# __pow__ **
# __and__ &
# __xor__ ^
# __or__ |

# Creamos otro ejemplo. Esta vez con palabras
# Queremos que cuando se dividan dos palabras se muestre la 
# primera en una línea, se dibuje una línea de guiones debajo
# y la segunda palabra por debajo de la línea de guiones
# Para que quede bien a la vista, la línea de guiones debería ser
# tan larga como la palabra más larga.
# Para ello creamos una lista de dos elementos con las longitudes
# de las dos palabras involucradas y calculamos el valor máximo
# que es número de veces que se repite el caracter guión
class Palabra:
 def __init__(self,texto):
  self.texto=texto
 def __truediv__(self,other):
  print(self.texto)
  print("-" * max([len(self.texto),len(other.texto)]))
  print(other.texto)
palabra1=Palabra("hola")
palabra2=Palabra("adios")
palabra1/palabra2
