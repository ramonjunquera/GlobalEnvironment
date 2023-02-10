#Autor: Ramón Junquera
#Fecha: 20221221
#Tema: Recursión

# Ejemplo de función recursiva para el cálculo de factoriales
def factorial(x):
  if x==1:
    return 1
  else:
    return x * factorial(x-1)
print(factorial(4)) #Respuesta: 24=4*3*2*1

# Es muy importante que una función recursiva contenga siempre 
# la condición del caso base.
# En el cálculo del factorial, el caso base es que el factorial
# de 1 es 1. No se necesitan más fórmulas para este caso.

# Ejemplo de función de cálculo del sumatorio a partir de 0
def suma(x):
  if x==1:
    return 1
  else:
    return x+suma(x-1)
print(suma(4)) #Resultado: 10=4+3+2+1

# Ejemplo de cálculo de la función de Fibonacci
def fibonacci(x):
  if x==0 or x==1:
    return 1
  else:
    return fibonacci(x-1)+fibonacci(x-2)
print(fibonacci(4)) #Resultado: 5=f(3)+f(2)=(f(2)+f(1))+(f(1)+f(0))=(f(2)+1)+(1+1)=(f(1)+f(0)+1)+2=(1+1+1)+2=3+2=5
