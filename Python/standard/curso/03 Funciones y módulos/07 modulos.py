#Autor: Ramón Junquera
#Fecha: 20220104

# Un módulo es código que debe ser importado para ser utilizado
# Una vez hecho con el comando import, se podrá llamar a los
# métodos que tenga definidos

# En este ejemplo importamos un módulo estándar llamado time que
# nos facilita la gestión de funciones de tiempo
# Tras importarlo, mostramos un mensaje, esperamos dos segundos,
# y mostramos un nuevo mensaje
import time
print("hola")
time.sleep(2)
print("adios")

# Para ahorrar memoria, no tenemos la necesidad de importar todos
# los métodos del módulo. Podríamos importar sólo el método que
# nos interesa
from time import sleep
print("hola")
#Ahora que sólo tenemos ese método, podemos llamarlo sin referenciar a su módulo
sleep(2)
print("adios")

# Podemos llamar a más de un método de un módulo, si lo separamos por comas
from time import sleep,strptime
print(strptime("27/05/1967","%d/%m/%Y"))

# En el momento de importar un método de un módulo le podemos poner
# un alias que nos facilite su identificación
from time import sleep as dormir
print("hola")
dormir(2)
print("adios")
# Atención. Si se importa un método con un alias, deberá ser llamado
# por el alias. Si se llama por el módulo.método dará un error como
# método no encontrado
