#Tema: Objeto lista
#Fecha: 20230111
#Autor: Ramón Junquera

# Una lista en Python es una mezcla de array y lista en C
# Se define incluyendo sus elementos en corchetes y separándolos por comas.
# Ejemplo de lista de cadenas
numeros=['uno','dos',"tres"]
print(numeros[0]) #Resultado: uno
print(numeros[1]) #Resultado: dos
print(numeros[2]) #Resultado: tres
# Ejemplo de lista de enteros
numeros=[5,4,3,2,1]
print(numeros[0]+numeros[1]) #Resultado: 9=5+4
# Una lista se puede imprimir completa
print(numeros) #Resultado: [5, 4, 3, 2, 1]
# También las listas de cadenas
numeros=['uno','dos','tres']
print(numeros) #Resultado: ['uno', 'dos', 'tres']
# También se pueden crear listas vacías
empty_list=[]
print(empty_list) #Resultado: []
# Las listas no tienen porqué contener elementos del mismo tipo
miLista=[1,'dos',3,4.0,True,'seis']
print(miLista) #Resultado: [1, 'dos', 3, 4.0, True, 'seis']
# Las lista se pueden anidar
miLista=[1,2,[11,12,13],4,5]
print(miLista) #Resultado: [1, 2, [11, 12, 13], 4, 5]
print(miLista[2]) #Resultado: [11, 12, 13]
print(miLista[2][0]) #Resultado: 11
# Las cadenas son consideradas listas de caracteres
palabra='hola'
print(palabra[1]) #Resultado: o
print(palabra[3]) #Resultado: a
# Se puede tomar un subconjunto de valores de una lista indicando el índice del 
# elemento inicial y el final menos uno
miLista=[1,2,3,4,5]
print(miLista[1:3]) #Resultado: [2, 3]
# También podemos omitir alguno de los dos valores
# Muestra todos los valores desde el indice 1 al final de la lista
miLista=[1,2,3,4,5]
print(miLista[1:]) #Resultado: [2, 3, 4, 5]
# Muestra todos los valores de la lista desde el principio hasta el indicado menos 1
miLista=[1,2,3,4,5]
print(miLista[:3]) #Resultado: [1, 2, 3]
# Para acceder a los elementos de una lista desde el final al principio utilizaremos
# valores negativos para los índices. Así el -1 será el último. El -2 el penúltimo.
# En el ejemplo se muestran los dos últimos valores
miLista=[1,2,3,4,5]
print(miLista[-2:]) #Resultado: [4, 5]

# Se pueden asignar los valores de una lista a varias variables en una sóla línea
miLista=['uno',2]
a,b=miLista
print(a) #Resultado: uno
print(b) #Resultado: 2
