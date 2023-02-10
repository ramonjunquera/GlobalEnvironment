#Autor: Ramón Junquera
#Fecha: 20230126

#Collección de ejemplos de gestión de archivos de texto y directorios

#Formato de apertura de archivos
#Para abrir un archivo utilizamos el comando open
#Su primer parámetro es el path y nombre del archivo
#El segundo parámetro indica la manera de abrirlo
#Las distintas posibilidades son
# "r" lectura de archivo de texto
# "w" escritura de archivo de texto
# "a" escritura añadiendo al final de un archivo de texto
# "rb" lectura de un archivo binario
# "wb" escritura de un archivo binario
# "ab" escritura añadiendo al final de un archivo binario

# Si se abre un archivo como escritura y no existe, se crea nuevo
# Si existe, se borrará

# Si se abre un archivo como escritura al final (append) y no existe,
# se creará nuevo

import os #Módulo de gestión de archivos

pathBase="d:/borrame" #Directorio de carpeta raíz

#Muestra el contenido de un directorio (función privada)
def _listDir(path):
  try:
    if not path.endswith("/"): #Si no termina con /
      path+="/" #...se lo añadimos
    fileNames=os.listdir(pathBase+path)
    for fileName in fileNames: #Recorremos todos los elementos
      entryStat=os.stat(pathBase+path+fileName)
      if entryStat.st_file_attributes & 32: #Si es un archivo...
        print("{} ({})".format(path+fileName,entryStat.st_size))
      else: #Si es un directorio...
        print("{} <DIR>".format(path+fileName))
        _listDir(path+fileName)
  except:
    return #Directorio inexistente o sin permisos

#Muestra el contenido de un directorio
def listDir(path):
  print("\nMostrando directorio "+path)
  _listDir(path)

#Crea un directorio
def createDir(path):
  print("\nCreando directorio: "+path)
  try:
    os.mkdir(pathBase+path)
    print("Directorio creado")
  except:
    print("Error creando directorio")

#Elimina un directorio
def removeDir(path):
  print("\nEliminando directorio: "+path)
  try:
    os.rmdir(pathBase+path)
    print("Directorio eliminado")
  except:
    print("Error eliminando directorio")

#Crea un archivo con un texto en su interior
def writeFile(fileName,message):
  print("\nCreando archivo: "+fileName+", con texto: ")
  print(message)
  try:
    file=open(pathBase+fileName,"w")
    charsWritten=file.write(message)
    print("Archivo creado")
    if charsWritten==len(message): print("Escritura correcta")
    else: print("Escritura incompleta")
  except:
    print("Error al crear el archivo")
  finally:
    file.close()
#Añade un texto al final de un archivo
def appendFile(fileName,message):
  print("\nAñadiendo a archivo: "+fileName+", el texto: "+message)
  try:
    file=open(pathBase+fileName,"a")
    file.write(message)
    print("Texto añadido")
  except:
    print("Error al añadir texto")
  finally:
    file.close()

#Borra un archivo
def deleteFile(fileName):
  print("\nBorrando archivo: "+fileName)
  try:
    os.remove(pathBase+fileName)
    print("Archivo borrado")
  except:
    print("Error al borrar archivo")

#Muestra el contenido de un archivo
#Lo carga completo en una variable
def readFile1(fileName):
  print("\nLeyendo archivo (completo): "+fileName)
  try:
    file=open(pathBase+fileName,"r")
    print("Leido desde archivo: "+file.read())
  except:
    print("Error al abrir el archivo")
  finally:
    file.close()

#Muestra el contenido de un archivo
#Lee su contenido carácter a carácter
def readFile2(fileName):
  print("\nLeyendo archivo (byte a byte): "+fileName)
  try:
    file=open(pathBase+fileName,"r")
    charRead="x"
    #Si seguimos leyendo un arhivo después de haberlo leido completamente,
    #no dará error. Simplemente no leerá nada
    while len(charRead)>0:
      charRead=file.read(1) #Sólo leemos un byte
      print(charRead,end="") #Eliminamos el carácter de fin de línea
    print() #Saltamos de línea para separar
  except:
    print("Error al abrir el archivo")
  finally:
    file.close()

#Muestra el contenido de un archivo
#Lee sus líneas en una lista
def readFile3(fileName):
  print("\nLeyendo archivo (en una lista): "+fileName)
  try:
    file=open(pathBase+fileName,"r")
    lines=file.readlines() #Lee cada línea en un item de una lista
    print("Archivo con {} lineas".format(len(lines)))
    lineIndex=0
    for line in lines:
      lineIndex+=1 #Aumentamos el contador de líneas
      #Mostramos los elementos de la lista de uno en uno
      #Quitamos el salto de línea final porque ya la tiene cada línea
      print("{} : {}".format(lineIndex,line),end="") 
  except:
    print("Error al abrir el archivo")
  finally:
    file.close()

#Renombra un archivo
def renameFile(fileName1,fileName2):
  print("\nRenombrando archivo "+fileName1+" a "+fileName2)
  try:
    os.rename(pathBase+fileName1,pathBase+fileName2)
    print("Archivo renombrado")
  except:
    print("Error al renombrar")

#Demo creación, lectura, adición, renombrado y eliminación de archivos en raíz
listDir("/")
writeFile("/hello1.txt","uno\ndos\ntres\n") #3 líneas 
writeFile("/hello2.txt","Hello ")
listDir("/")
readFile3("/hello1.txt") #Leemos archivo en una lista de líneas
deleteFile("/hello1.txt") #Borrado de archivo existente
deleteFile("/foo.txt") #No se puede borrar un archivo inexistente
readFile1("/hello2.txt") #Leemos archivo de una vez
appendFile("/hello2.txt","World")
renameFile("/hello2.txt","/foo.txt")
readFile2("/foo.txt") #Leemos archivo byte a byte
deleteFile("/foo.txt")
listDir("/")

#Demo creación y eliminación de directorio
listDir("/x23") #Directorio inexistente
listDir("/")
createDir("/mydir")
listDir("/")
removeDir("/mydir")
listDir("/")

#Demo creación, lectura, adición, renombrado y eliminación de archivos en directorio
createDir("/mydir")
writeFile("/mydir/hello1.txt","Hi!")
writeFile("/mydir/hello2.txt","Hello ")
listDir("/")
deleteFile("/mydir/hello1.txt") #Borrado de archivo existente
deleteFile("/mydir/foo.txt") #No se puede borrar un archivo inexistente
readFile1("/mydir/hello2.txt")
appendFile("/mydir/hello2.txt","World")
renameFile("/mydir/hello2.txt","/mydir/foo.txt")
readFile1("/mydir/foo.txt")
listDir("/mydir")
removeDir("/mydir") #Error porque no está vacío
deleteFile("/mydir/foo.txt")
removeDir("/mydir") #Ahora sí
listDir("/")
