#Tema: Llamadas a funciones
#Fecha: 20230110
#Autor: Ramón Junquera

#Nota: Necesario conocimiento de diccionarios

#Se puede llamar a funciones desde otras funciones
def funcion1():
  print("Hola mundo")
def funcion2(saludo,nombre):
  print(saludo,nombre)
  funcion1()
funcion2("Hasta luego","Lucas")

#Se puede pasar la referencia de un función como parámetro
def suma(a,b):
  return a+b
def resta(a,b):
  return a-b
def sumaresta(funcion,a,b):
  return funcion(a,b)
print(sumaresta(suma,3,2)) #Resultado: 5
print(sumaresta(resta,3,2)) #Resultado: 1
print(locals())
print(globals())

#La función locals() devuelve un diccionario con las funciones locales (definidas en el propio módulo). La clave es el
#nombre de la función y el valor es la referencia de la función.
#De la misma manera, la función globals() devuelve el diccionario con todas las funciones definidas, tanto en el propio
#módulo (locales) como en otros módulos importados previamente.
#En este ejemplo tanto locals() como globals() devuelven el mismo contenido porque no se importa
#ningún módulo
if "suma" in locals():
  print("suma es una función existente")
else:
  print("La función suma no está definida")
if "division" in locals():
  print("division es una función existente")
else:
  print("La función division no está definida")

#Podemos localizar por clave cualquier función en el diccionario y tomar su valor para llamar
# a su función
print(locals()["suma"](3,2)) #Resultado: 5






  


