#Autor: Ramón Junquera
#Fecha: 20221223

# Pitonicidad. Operador ternario

# Python permite aplicar condidicionales en una misma línea como lo hacen
# otros lenguajes
# En C se escribiría: a=(b>5?6:7)
# En Python: a= 6 if b>5 else 7

# Se llama operador ternario porque tiene 3 argumentos:
# El valor por defecto
# La condición
# El valor si no cumple la condición

# Veamos un ejemplo
b=9
print(6 if b>5 else 7)
b=3
print(6 if b>5 else 7)

# Otro ejemplo
status=1
msg="Logout" if status==1 else "Login"
print(msg)