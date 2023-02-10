#Autor: Ramón Junquera
#Fecha: 20221221

# Métodos mágicos. Comparaciones

# Python también implementa métodos mágicos para las comparaciones
# Este es el listado:
# __lt__ < (less than)
# __le__ <= (less or equal)
# __eq__ == (equal)
# __ne__ != (not equal)
# __gt__ > (greater than)
# __ge__ >= (greater or equal)

# Creamos un ejemplo con cadenas, de la cual sólo tendremos en 
# cuenta su longitud
# Sólo implementaremos el método == y >
class Palabra:
 def __init__(self,texto):
  self.texto=texto
 def __eq__(self,other): # ==
  return len(self.texto)==len(other.texto)
 def __gt__(self,other): # >
  return len(self.texto)>len(other.texto)
palabra1=Palabra("hola")
palabra2=Palabra("adios")
print("{} es igual que {}?. {}".format(palabra1.texto,palabra2.texto,palabra1==palabra2))
print("{} es mayor que {}?. {}".format(palabra1.texto,palabra2.texto,palabra1>palabra2))
print("{} es mayor que {}?. {}".format(palabra2.texto,palabra1.texto,palabra2>palabra1))
print("{} es distinta que {}?. {}".format(palabra1.texto,palabra2.texto,palabra1!=palabra2))
# Todo es correcto excepto la última línea, porque nosotros no hemos implementado
# el método __ne__ en nuestra clase!
# Esto ocurre porque si no se ha definido __ne__ pero sí __eq__, se toma por defecto
# que __ne__ es justo lo contrario de __eq__
# Es el único caso en el que se toma un método no creado por defecto