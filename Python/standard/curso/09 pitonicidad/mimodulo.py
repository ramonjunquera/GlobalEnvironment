#Autor: Ramón Junquera
#Fecha: 20221223

# Ejemplo para 06 __main__

def suma(a,b):
 return a+b

if __name__=="__main__":
 print("Estoy siendo ejecutado desde la línea de comandos")
else:
 print("Estoy siendo ejecutado desde dentro del modulo {}".format(__name__))
