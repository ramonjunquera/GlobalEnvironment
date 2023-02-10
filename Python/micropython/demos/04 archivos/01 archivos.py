#Autor: Ramón Junquera
#Fecha: 20221229
#Ejemplo de gestión de archivos y directorios

PATH_BASE="/data" #Directorio de carpeta raíz. Debe existir

import os #Módulo de gestión de archivos

#Muestra el contenido de un directorio (función privada)
def _listDir(path):
  try:
    #Podríamos utilizar os.listdir() para obtener una tupla con los nombres de las entradas
    #pero como necesitaremos más información (tipo y tamaño), utilizamos os.ilistdir().
    #ilistdir es un iterator de tuplas con la información de las entradas del directorio
    #Tiene 4 elementos:
    # 0 : name
    # 1 : type. 0x4000=directory, 0x8000=file
    # 2 : inode
    # 3 : size
    for entry in os.ilistdir(PATH_BASE+path):
      if entry[1] & 0x8000: #Si es un archivo...
        print("{} ({})".format(path+entry[0],entry[3])) #Nombre y tamaño
      else: #Si es un directorio...
        print("{} <DIR>".format(path+entry[0]))
        _listDir(path+entry[0]+"/")
  except:
    return #Directorio inexistente o sin permisos

#Muestra el contenido de un directorio
def listDir(path):
  if not path.endswith("/"):
    path+="/" #Un path siempre debe terminar con /
  print("\nMostrando directorio "+path)
  _listDir(path)

#Crea un directorio
def createDir(path):
  print("\nCreando directorio: "+path)
  try:
    os.mkdir(PATH_BASE+path)
    print("Directorio creado")
  except:
    print("Error creando directorio")

#Elimina un directorio
def removeDir(path):
  print("\nEliminando directorio: "+path)
  try:
    os.rmdir(PATH_BASE+path)
    print("Directorio eliminado")
  except:
    print("Error eliminando directorio")

#Crea un archivo con un texto en su interior
def writeFile(fileName,message):
  print("\nCreando archivo: "+fileName+", con texto: ")
  print(message)
  try:
    file=open(PATH_BASE+fileName,"w")
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
    file=open(PATH_BASE+fileName,"a")
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
    os.remove(PATH_BASE+fileName)
    print("Archivo borrado")
  except:
    print("Error al borrar archivo")

#Muestra el contenido de un archivo
#Lo carga completo en una variable
def readFile1(fileName):
  print("\nLeyendo archivo (completo): "+fileName)
  try:
    file=open(PATH_BASE+fileName,"r")
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
    file=open(PATH_BASE+fileName,"r")
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
    file=open(PATH_BASE+fileName,"r")
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
    os.rename(PATH_BASE+fileName1,PATH_BASE+fileName2)
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
removeDir("/mydir") #No se puede eliminar un directorio con contenido
deleteFile("/mydir/foo.txt")
removeDir("/mydir")
listDir("/")
