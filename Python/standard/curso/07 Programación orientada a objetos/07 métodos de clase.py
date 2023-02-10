#Autor: Ramón Junquera
#Fecha: 20221221

# Métodos de clase
# Anteriormente hemos visto la diferencia entre una variable de
# instancia y de clase.
# Con los métodos ocurre lo mismo.
# Los métodos que hemos definido hasta ahora son todos de instancia
# Y deben ser llamados desde una instancia
# Crearemos un ejemplo que incluye in método de clase
# Los métodos de clase pueden ser llamados directamente desde la
# clase o desde la instancia. En C++ son los métodos estáticos.
# En el ejemplo creamos una clase que gestiona coches
# En el constructor guardamos los valores de color y marca
# Pero la clase tiene definido un conjunto a nivel de clase con
# los colores permitidos.
# Cuendo creamos la instancia se comprueba que el color sea
# alguno de los permitidos. Si no es así se genera una excepción.
# Cremos un método a nivel de clase que nos permite comprobar si
# un color está permitido.
# No necesitamos tener un coche (una instancia) para preguntar si
# el color que le queremos asignar está permitido.
# La definición es igual que cualquier otro método, excepto que
# se utiliza el decorador @classmethod justo antes
# Puesto que el parámetro que se le pasa ya no es el de la instancia
# no le llamaremos self, sino cls de clase. De todas maneras ambas
# palabras son convenciones y podríamos utilizar cualquiera otras
class Coche:
 grupoColores={"rojo","verde","negro"} #Variable de clase
 def __init__(self,color,marca): #Constructor
  self.marca=marca
  if color not in self.grupoColores:
   raise ValueError("Color no permitido")
  self.color=color
 @classmethod
 def color_valido(cls,color): #Método de clase
  return color in cls.grupoColores
miCoche=Coche("rojo","Seat")
color="azul"
print("Se permite el color {}?. {}".format(color,Coche.color_valido(color)))

# Desarrollamos otro ejemplo
# Esta vez el método de clase nos servirá para crear una instancia
# de la clase Rectangulo pero para un cuadrado al cual sólo
# le pasaremos el valor del lado
class Rectangulo:
 def __init__(self,base,altura):
  self.base=base
  self.altura=altura
 def area(self):
  return self.base*self.altura
 @classmethod
 def nuevo_cuadrado(cls,lado):
  # Devuelve una instacia de la misma clase actual, pero los
  # valores de base y altura son iguales al lado
  return cls(lado,lado)
# Llamamos al método de clase de Rectangulo que ya devuelve una
# instancia con los parámetros correctos
miCuadrado=Rectangulo.nuevo_cuadrado(5)
# Comprobamos que la instancia es correcta y el cálculo del
# área también
print(miCuadrado.area())

# Un método estático es un método que no tiene ninguna relación
# con la clase ni necesita nada de ella. Es como una función normal.
# Retomando el ejemplo anterior incluiremos un método estático
# que devuelva la fórmula del área.
# Para ello no necesitamos ninguna información de la clase, puesto
# que la fórmula siempre será la misma.
class Rectangulo:
 def __init__(self,base,altura):
  self.base=base
  self.altura=altura
 def area(self):
  return self.base*self.altura
 @staticmethod
 def formula():
  return "base * altura"
# Creamos una instancia de Rectangulo
miRectangulo=Rectangulo(5,8)
# Comprobamos que funciona el método de instancia
print(miRectangulo.area())
# Comprobamos que funciona el método estático llamado
# desde la clase
print(Rectangulo.formula())
# Comprobamos que funciona el método estático llamado
# desde la instancia
print(miRectangulo.formula())

# Un método estático podría tener parámetros igual que cualquier
# otra función
