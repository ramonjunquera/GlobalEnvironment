#Autor: Ramón Junquera
#Fecha: 20221221

# Propiedades
# Son métodos que se llaman como si fuesen variables de instancia
# La ventaja es que al ser métodos, su valor puede ser calculado
# en ese momento.
# Se utiliza principalmente para que se comporten como variables
# de sólo lectura
# Ejemplo. Tenemos la clase Rectangulo con su constructor que
# almacena las dimensiones
# Tenemos una función de instancia que calcula su área
# La convertimos en una propiedad añadiendo previamente el 
# decorador @property
# Ahora cada vez que llamemos a una variable que tenga el mismo
# nombre que el método, se ejecutará y devolverá su resultado
# Desde el punto de vista del consumidor de la clase se trata de
# una variable de instancia a la que no se le puede asignar un
# valor porque da error.
class Rectangulo:
 def __init__(self,base,altura):
  self.base=base
  self.altura=altura
 @property
 def area(self):
  return self.base*self.altura
# Creamos una instancia de Rectangulo
miRectangulo=Rectangulo(5,8)
# Comprobamos que funciona el método de instancia
print(miRectangulo.area)
# Si intentamos asignarle un valor a area:
# miRectangulo.area=23
# Obtendremos la excepción AttributeError

# Hemos aprendido a crear propiedades que pueden ser leidas pero
# no escritas.
# para que una propiedad pueda ser escrita, primero debe ser
# creada como hemos visto hasta ahora.
# Pero además hay que crear un decorador con el mismo nombre
# que la propiedad seguido por .setter para poder fijar un valor
# A continuación escribiremos el método de instancia con dos
# parámetros. El primero para el self y el segundo para el valor
# asignado.
# Este método debe tener el mismo nombre que el de la propiedad
# Usamos el ejemplo anterior, al que añadimos la propiedad base
# que permite consultar el valor de la base del rectángulo definido
# en la instancia.
# Con el mismo nombre creamos el .setter para poder escribir en 
# esta propiedad.
# En el ejemplo se crea la instancia, se comprueba el área (con la
# propiedad), se cambia la base y se vuelve a comprobar el área
# para ver que el valor ha cambiado
class Rectangulo:
 def __init__(self,base,altura):
  self._base=base
  self._altura=altura
 @property
 def area(self):
  return self._base*self._altura
 @property
 def base(self):
  return self._base
 @base.setter
 def base(self,base):
  self._base=base
miRectangulo=Rectangulo(5,8) # Creamos una instancia de Rectangulo
print(miRectangulo.area) # Mostramos el área. Resultado: 40
miRectangulo.base=3 # Cambiamos el valor de la base
print(miRectangulo.area) # Comprobamos que también ha cambiado el área. Resultado: 24
