#Autor: Ramón Junquera
#Fecha: 20221221

# Ocultación de información
# En Python no existe la clausula private.
# Todos los métodos y variables de una clase son públicos.
# Lo único que se pude hacer es utilizar una nomenclatura ya
# pactada para indicar qué métodos o variables son de uso 
# interno. Aun así seguirán siendo accesibles.

# Esta nomenclatura es que se utilice el guión bajo como
# prefijo a cualquier variable o método que no debería ser
# utilizado públicamente.

# El único efecto real es que cuando se hace un import de
# un módulo, todos los objetos que comiencen por un guión
# bajo, no se tendrán en cuenta.

# En el siguiente ejemplo creamos una clase que gestiona un valor
# numérico. Tiene métodos para aumentar y disminuir ese valor en
# una unidad. El valor es privado (tiene prefijo _)
# También tenemos un método para mostar el valor
# Operamos y finalmente mostramos el valor tanto por método como
# por acceso directo a la variable
class MyClass:
 def __init__(self):
  self._elements=0 # Inicialmente comenzamos con 0 elementos
 def show(self):
  print(self._elements)
 def add(self):
  self._elements+=1
 def remove(self):
  self._elements-=1
myClass=MyClass() # Instanciamos
myClass.show() # Mostramos 0 elementos
myClass.add() # Ya tenemos 1
myClass.add() # Ya tenemos 2
myClass.remove() # Ahora sólo queda 1
myClass.add() # Volvemos a subir a 2
myClass.show() # Mostramos por método
print(myClass._elements) # Mostramos por variable privada

# Si en vez de utilizar un sólo guión, usamos 2, tenemos un objeto
# fuertemente "fuertemente privado". En este caso en Python, estos
# objetos ya no serían accesibles desde fuera de la clase.
# Repetimos el ejemplo anterior pero con doble guión bajo para la
# variable elements.
# El resultado es el mismo
class MyClass:
 def __init__(self):
  self.__elements=0 # Inicialmente comenzamos con 0 elementos
 def show(self):
  print(self.__elements)
 def add(self):
  self.__elements+=1
 def remove(self):
  self.__elements-=1
myClass=MyClass() # Instanciamos
myClass.show() # Mostramos 0 elementos
myClass.add() # Ya tenemos 1
myClass.add() # Ya tenemos 2
myClass.remove() # Ahora sólo queda 1
myClass.add() # Volvemos a subir a 2
myClass.show() # Mostramos por método
print(myClass.__elements) # Mostramos por variable privada. Provoca un error porque no tenemos permiso