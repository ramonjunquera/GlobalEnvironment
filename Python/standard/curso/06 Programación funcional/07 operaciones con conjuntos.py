#Autor: Ramón Junquera
#Fecha: 20221221

# Operaciones con conjuntos

# Operador intersección. Se representa por &. Igual que método intersection
# Devuelve los elementos comunes a dos conjuntos
# Resultado = {4, 5}
conjunto1={1,2,3,4,5}
conjunto2={8,7,6,5,4}
print(conjunto1 & conjunto2) #Resultado: {4, 5}
print(conjunto1.intersection(conjunto2)) #Resultado: {4, 5}

# Operador diferencia. Se representa por -. Igual que método difference
# Devuelve los elementos que se encuentran en el primer conjunto
# pero no en el segundo
# Resultado = {1, 2, 3}
conjunto1={1,2,3,4,5}
conjunto2={8,7,6,5,4}
print(conjunto1 - conjunto2) #Resultado: {1, 2, 3}
print(conjunto1.difference(conjunto2)) #Resultado: {1, 2, 3}

# Operador de diferencia simétrica. Se representa por ^
# Devuelve los elementos de ambos conjuntos que no son comunes
# Resultado = {8, 1, 2, 3, 6, 7}
conjunto1={1,2,3,4,5}
conjunto2={8,7,6,5,4}
print(conjunto1 ^ conjunto2) #Resultado: {1, 2, 3, 6, 7, 8}
print(conjunto1.symmetric_difference(conjunto2)) #Resultado: {1, 2, 3, 6, 7, 8}

# Nota: No existe la operación de unión de conjuntos como +
# Se debe utilizar el método union
conjunto1={1,2,3,4,5}
conjunto2={8,7,6,5,4}
print(conjunto1.union(conjunto2)) #Resultado: {1, 2, 3, 4, 5, 6, 7, 8}
