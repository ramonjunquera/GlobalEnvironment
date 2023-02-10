#Tema: Objeto tupla
#Fecha: 20230124
#Autor: Ramón Junquera

# Una tupla es una lista de constantes
# Por lo tanto, una vez definida no admite modificaciones.
# No se podrán añadir, eliminar o modificar elementos.
# Sólo se eliminarla y sustituirla completamente por otra.
# Se diferencian porque se definen con paréntesis y no con corchetes
# Por lo demás, pueden ser tratadas igual que las listas

# Cálculo del número de elementos
miTupla=(1,2,3,4,5)
print(len(miTupla)) #Número de elementos de la tupla. Respuesta: 5
del miTupla

# Como las listas, los datos pueden ser heterogéneos
miTupla=(1,'dos',3.0,(5,6,7))
print(miTupla) # Mostramos la tupla completa
print(miTupla[1]) # Mostramos sólo un elemento de la tupla

# También se pueden crear tuplas sin utilizar paréntesis
miTupla=1,2,3,4,5
print(miTupla)

# Se puede crear un tupla vacía (aunque no es muy útil)
miTupla=()
print(miTupla)

# Si estamos interesados en modificar sus elementos tenemos que convertirla en una lista
#Para ello utilizamos el comando list
miTupla=("uno","dos","tres")
miLista=list(miTupla) #Convertimos a lista
miLista[1]="cuatro" #Ahora podemos mmoificar sus elementos
print(miLista) #Resultado: ['uno', 'cuatro', 'tres'] 

