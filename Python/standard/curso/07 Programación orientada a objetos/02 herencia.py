#Tema: Herecia de clases
#Fecha: 20221221
#Autor: Ramón Junquera

# Herencia. Es la manera de compartir funcionalidades entre clases.

# Se puede crear una clase tomando como base otra existente
# Se mantendrán todas las propiedades (métodos y variables) de la clase original,
# pero podremos añadir nuevas o modificar (sobreescribir) las existentes.

# En el siguiente ejemplo definimos una clase base llamada Animal
# Tiene un par de variables de instancia: color y patas
# A continuación definimos la clase Perro basada en Animal a la cual le añadimos
# un nuevo método llamado ladra.
# Este método sólo afectará a la clase Perro, no Animal.
# Repetimos lo mismo pero creando una clase Gato con el método maulla
# Después creamos una instancia de las clases heredadas y probamos los nuevos
# métodos implementados
class Animal:
 def __init__(self,color,patas):
  self.color=color
  self.patas=patas
class Perro(Animal):
 def ladra(self):
  print("guau")
class Gato(Animal):
 def maulla(self):
  print("miau")
koko=Perro("marron",4)
minino=Gato("blanco",4)
koko.ladra()
minino.maulla()

# A las clases heredadas se les llama subclases
# Y las clases que tienen herencias se llaman superclases
# En el ejemplo anterior, Animal es la superclase y Perro y Gato subclases.

# Cuando en una subclase se define un método con el mismo nombre
# que la superclase, se sobreescribe. Se tendrá en cuenta el nuevo
# método definido en la subclase.
class SuperClase:
 def miMetodo(self):
  print("Soy el método de la SuperClase")
class SubClase(SuperClase):
 def miMetodo(self):
  print("Soy el método de la SubClase")
miSuperClase=SuperClase()
miSubClase=SubClase()
miSuperClase.miMetodo()
miSubClase.miMetodo()  

# La herencia puede tener más de dos niveles
# En el siguiente ejemplo se definen 3 clases.
# La segunda hereda de la primera y la tercera de la segunda
# Por lo tanto, la tercera tiene los métodos de todas las superclases
# superiores
# No está permitida la herencia circular
class Clase1:
 def metodo1(self):
  print("Soy el método 1")
class Clase2(Clase1):
 def metodo2(self):
  print("Soy el método 2")
class Clase3(Clase2):
 def metodo3(self):
  print("Soy el método 3")
miClase3=Clase3()
miClase3.metodo1()
miClase3.metodo2()
miClase3.metodo3()

# Cuando se sobreescribe un método en una subclase, se pierde la
# posibilidad de seguir accediendo al método de la superclase.
# Para que esto no ocurra, tenemos la función super() que
# representa el objeto de la superclase. Gracias a él podemos
# seguir accediendo a todos sus métodos
# En el siguiente ejemplo definimos una superclase con un método
# Creamos una subclase que herede la anterior
# La subclase tiene un metodo con el mismo nombre que la superclase
# por lo tanto se sobreescribe y se toma la nueva funcionalidad
# Pero se añade un segundo método que hace referencia al 
# método de la superclase
class SuperClase:
 def metodo1(self):
  print("Soy el método de la SuperClase")
class SubClase(SuperClase):
 def metodo1(self):
  print("Soy el método de la SubClase")
 def metodo2(self):
  super().metodo1()
miSubClase=SubClase()
miSubClase.metodo1()
miSubClase.metodo2()
