#Autor: Ramón Junquera
#Fecha: 20221219

# Conversión de cadenas a números enteros
print(int('2')+int('3')) #Resultado: 5
print(int('2'+'3')) #Resultado: 23
# Pide un número, lo convierte a entero y lo multiplica por dos
# Si la cadena introducida no se puede convertir a entero dará error
print(int(input('Escribe un numero:'))*2)

# Convierte a flotante
print(float('4')*2) #Resultado: 8.0
# Si el número introducido es 2, el resultado es 210210.0
print(float("210" * int(input("Escribe un numero:"))))

# Se puede convertir un número en una cadena con la función str
print('3' + str(2+5)) #Resultado: 37

# También se convierte números flotantes, pero lo hace con decimales
print(str(3.45) + ':' + str(2.0+5)) #Resultado: 3.45:7.0
