#Autor: Ramón Junquera
#Fecha: 20221223

# Pitonicidad. __name__

# En Python tenemos dos manera de ejecutar el código: como script
# y como módulo

# Como script es cuado escribimos y ejecutamos código desde la línea de
# comando.
# Como mádulo es cuando cargamos un módulo y llamamos a su contenido

# Existe una variable privada llamada __name__
# Que siempre tiene el valor del módulo al que pertenece

# Si mostramos su contenido desde la línea de comandos:
print(__name__)
# Tenemos que su valor es __main__ que es el nombre que se le da al
# módulo de la línea de comandos

# Si hacemos lo mismo desde un módulo, __name__ tendrá el nombre del 
# módulo

# Esto nos sirve para poder distinguir si el código que se está ejecutando
# se está haciendo desde la línea de comandos o desde un módulo

# En esta misma carpeta tenemos un módulo llamado mimodulo.py
# que podemos abrir para mirar su contenido.
# Contiene una simple función llamada suma
# Pero además hace una comprobación de la variable __name__
# escribiendo así si es un módulo o no

# Si mantenemos el archivo mimodulo.py en la misma carpeta que este ejemplo
# podremos ejecutar la siguiente llamada:
import mimodulo
print(mimodulo.suma(3,2)) #Resultado: Estoy siendo ejecutado desde dentro del modulo mimodulo

