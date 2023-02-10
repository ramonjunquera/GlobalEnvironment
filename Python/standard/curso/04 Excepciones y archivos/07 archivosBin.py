#Autor: Ramón Junquera
#Fecha: 20230127

#Collección de ejemplos de gestión de archivos binarios

import os #Módulo de gestión de archivos

pathBase="d:/borrame/" #Directorio de carpeta raíz

#Crea un archivo
#  fileName : nombre del archivo
#  values : lista/tupla con valores para contenido
def createFileTupla(fileName,values):
  print("\nCreando archivo con tupla",fileName)
  try:
    file=open(pathBase+fileName,"wb") #Borra cualquier archivo existente con el mismo nombre
    for value in values:
      #Escribimos los elementos de uno en uno
      #Antes los convertimos a bytes
      #Indicamos cuánto deden ocupar y el tipo de codificación
      #file.write(value.to_bytes(1,byteorder='big'))
      file.write(value.to_bytes(1,"big")) #Es más corto
    print("Archivo creado")
  except:
    print("Error al crear el archivo")
  finally:
    file.close()

#Muestra el contenido de un archivo binario cargándolo completamente en memoria
def readFile1(fileName):
  print("\nLeyendo archivo completo de",fileName)
  try:
    file=open(pathBase+fileName,"rb") #Sólo lectura
    values=file.read()
    for value in values:
      print(value,end=" ")
    print()
  except:
    print("Error al leer el archivo")
  finally:
    file.close()

#Muestra el contenido de un archivo
#Lee su contenido byte a byte
def readFile2(fileName):
  print("\nLeyendo archivo (byte a byte) de",fileName)
  file=open(pathBase+fileName,"rb") #Sólo lectura
  byteRead=file.read(1)
  while(len(byteRead)>0): #Si se ha leido algo
    #Es necesario convertir el byte a entero para poder operar con él
    print(int.from_bytes(byteRead,"big"),end=" ")
    byteRead=file.read(1)
  print()
  file.close()

#Borra un archivo
def deleteFile(fileName):
  print("\nBorrando archivo: "+fileName)
  try:
    os.remove(pathBase+fileName)
    print("Archivo borrado")
  except:
    print("Error al borrar archivo")

#Append a un archivo binario
#  fileName : nombre del archivo
#  values : lista/tupla con valores para contenido
def appendFileTupla(fileName,values):
  print("\nAñadiendo a archivo con tupla",fileName)
  try:
    file=open(pathBase+fileName,"ab") #Append binario
    for value in values:
      file.write(value.to_bytes(1,"big"))
    print("Archivo añadido")
  except:
    print("Error al añadir al archivo")
  finally:
    file.close()

#Update a un archivo binario
#  fileName : nombre del archivo
#  values : lista/tupla con valores para contenido
#  pos : posición a partir de la que se hace la actualización
def updateFileTupla(fileName,values,pos):
  print("\nActualizando archivo {} en posición {}".format(fileName,pos))
  try:
    file=open(pathBase+fileName,"rb+") #Abre como lectura/escritura
    file.seek(pos)
    for value in values:
      file.write(value.to_bytes(1,"big"))
    print("Archivo actualizado")
  except:
    print("Error al actualizar archivo")
  finally:
    file.close()

#Demo
createFileTupla("test.bin",(11,22,33,44,55))
readFile1("test.bin") #Lectura completa
appendFileTupla("test.bin",(66,77))
readFile2("test.bin") #Lectura secuencial
updateFileTupla("test.bin",(20,30),1)
readFile1("test.bin") #Lectura completa
deleteFile("test.bin")


#Pendiente
# https://www.bestprog.net/en/2020/04/30/python-binary-files-examples-of-working-with-binary-files/

