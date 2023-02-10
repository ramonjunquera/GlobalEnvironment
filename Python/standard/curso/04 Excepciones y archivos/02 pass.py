#Tema: Comando pass
#Fecha: 20230110
#Autor: Ramón Junquera

#El comando pass sirve para "no hacer nada"!
#Porqué necesitamos un comando que no nace nada?
#Porque tenemos estructuras que siempre deben contener algo, sino darán error.
#Por ejemplo las estructuras de excepciones

#Supongamos que tenemos la siguiente función
def division(n,d):
  return n/d
#Funcionará correctamente siempre que el parámetro del denominador no sea cero
#En ese caso se producirá una excepción
#Como queremos evitarlo introducimos una estructura para el controld excepciones
#No queremos que haga nada especial cuando se produzca una excepción. Simplemente
#que no se detenga la ejecución.
#Lo solventamos con pass
def division(n,d):
  try:
    return n/d
  except:
    pass
print(division(4,2)) #Resultado: 2.0
print(division(4,0)) #Resultado: None porque nunca se devuelve nada

#Aunque lo más práctico suele ser decidir una valor que devolver cuando el denominador sea cero
#Por ejemplo: cero
def division(n,d):
  try:
    return n/d
  except:
    return 0.0
print(division(4,2)) #Resultado: 2.0
print(division(4,0)) #Resultado: 0.0

#Nota el siguiente ejemplo requiere conocimiento de clases

#Podríamos necesitar la creación de una clase vacía que posteriormente la utilizamos para heredar
#y generar otras clases derivadas. El problema es que no se puede crear una clase vacía si no es con pass
class Padre:
  pass
class Hijo(Padre):
  def diHola():
    print("hola")
miHijo=Hijo
miHijo.diHola()

