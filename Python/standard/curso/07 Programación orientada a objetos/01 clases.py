#tema: Clases
#Fecha: 20230110
#Autor: Ramón Junquera

# Clases
# Al igual que en otros lenguajes de programación una clase es un
# objeto que contiene sus propias funciones (métodos) y sus 
# propias variables (valores)

# Los métodos en una función se declaran como las funciones, con def
# Es obligatorio que todos los métodos de instancia tengan como
# primer parámetro self.
# Este parámetro nunca se utiliza explícitamente al llamar
# al método desde una clase instanciada.

# En el siguiente ejemplo se define una clase con un sólo método
# A continuación se instancia y por último se llama al método
class MiClase:
  def miMetodo(self):
    print("Soy el único método")
instancia=MiClase()
instancia.miMetodo() #Resultado: Soy el único método

# Como en otros lenguajes de programación se puede definir un 
# método contructor. Es el método al que se llama automáticamente
# cuando se instancia una clase.
# En Python el constructor se llama __init__
# Como hemos visto antes, no es obligatorio definirlo

# En el siguiente ejemplo creamos una clase llamada Animal
# Cuando se instancia se llama al método __init__ (constructor)
# y se solicita el color y el número de patas, que se guardan 
# en variables de instancia.
# Estas variables se escriben como self.nombre_de_variable
# y pueden ser accedidas desde la instancia de la clase.
# A continuación creamos una instancia de la clase dando
# valores a sus variables
# Por último mostramos que las variables de la instancia se han
# recogido correctamente
class Animal:
  def __init__(self,color,patas):
    self.color=color
    self.patas=patas
koko=Animal("marron",4)
print("color = {}, patas = {}".format(koko.color,koko.patas)) #Resultado: color = marron, patas = 4  

# Además del método __init__ podemos crear nuestros propios métodos
class Animal:
  def __init__(self,color,patas):
    self.color=color
    self.patas=patas
  def doble_de_patas(self):
    return 2*self.patas
  def di_hola(self):
    print("hola")
koko=Animal("marron",4)
print(koko.doble_de_patas()) #Resultado: 8
koko.di_hola() #Resultado: hola

# Además de las variables de instancia, tenemos las variables
# de clase.
# Estas variables no dependen de la instancia, por lo tanto se puede
# acceder a ellas consultando tanto la instancia como 
# directamente la clase.
# Estas variables son definidas sin el prefijo self.
# Definimos una clase simple que sólo tiene una variable de clase
# Después accedemos a su valor a través de la instancia y de la clase
class MiClase:
  mi_variable_de_clase=2 # Definimos una variable de clase
instancia1=MiClase() # Creamos una instancia de la clase
print("Desde la instancia = {}".format(instancia1.mi_variable_de_clase)) #Resultado: Desde la instancia = 2
print("Desde la clase = {}".format(MiClase.mi_variable_de_clase)) #Resutlado: Desde la clase = 2
# Comprobamos que obtenemos el mismo valor tanto si accedemos a
# la variable por la instancia como por la clase

# Cuando se instancia una clase se crea un juego de variables de 
# instancia. Por defecto las variables de clase no están 
# incluidas en la instancia. Cuando se consultan se va a buscar
# su valor a la clase
# Si tenemos varias instancias de una clase y modificamos el valor
# de una variable de clase, afectará a todas las instancias que
# no la tienen incluida en los variables de instancia
class MiClase:
  mi_variable_de_clase=2 # Definimos una variable de clase
instancia1=MiClase() # Creamos una instancia de la clase
instancia2=MiClase() # Creamos una segunda instancia de la clase
print("instancia1={},instancia2={},clase={}".format(instancia1.mi_variable_de_clase,instancia2.mi_variable_de_clase,MiClase.mi_variable_de_clase)) #Resultado: instancia1=2,instancia2=2,clase=2
MiClase.mi_variable_de_clase=5 # Actualizamos el valor de la variable de clase
print("instancia1={},instancia2={},clase={}".format(instancia1.mi_variable_de_clase,instancia2.mi_variable_de_clase,MiClase.mi_variable_de_clase)) #Resultado: instancia1=5,instancia2=5,clase=5

# Para incluir una variable de clase en el juego de variables de
# instancia sólo tenemos que darle valor desde la instancia.
# A partir de ese instante, la variable se leerá desde la instancia
# Y los cambios a nivel de clase no le afectarán
class MiClase:
  mi_variable_de_clase=2 # Definimos una variable de clase
instancia1=MiClase() # Creamos una instancia de la clase
instancia2=MiClase() # Creamos una segunda instancia de la clase
print("instancia1={},instancia2={},clase={}".format(instancia1.mi_variable_de_clase,instancia2.mi_variable_de_clase,MiClase.mi_variable_de_clase)) #Resultado: instancia1=2,instancia2=2,clase=2
instancia1.mi_variable_de_clase=4 # Cambiamos el valor en la instancia1
MiClase.mi_variable_de_clase=5 # Actualizamos el valor de la variable de clase
print("instancia1={},instancia2={},clase={}".format(instancia1.mi_variable_de_clase,instancia2.mi_variable_de_clase,MiClase.mi_variable_de_clase)) #Resutlado: instancia1=4,instancia2=5,clase=5
