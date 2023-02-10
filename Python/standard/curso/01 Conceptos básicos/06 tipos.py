#Autor: Ramón Junquera
#Fecha: 20230104

#Python es un lenguaje no tipado
#No es necesario definir el tipo de variable al crearlas. Se deduce del contexto.
#Una misma variable podrá tener distintos tipo a los largo de su vida
a="Pepe"
print(a)
a=23
print(a)
a=3.14159
print(a)

#Nomenclatura
#Por convenio, las variables se nombran en minúsculas.
#Si el nombre de la variable contiene más de una palabra, se separán por subguiones
mi_variable_favorita=33
#Una variable nunca puede comenzar por un número
#Si una variable comienza por un subguión, se considera que es privada y debe estar oculta
#Si comienza por doble subguión se considera 'muy privada'

#Constantes
#En Python no existen las constantes
#De todas maneras, para diferenciar una variable que no debería cambiar su valor se
#nombran igual que las variables, pero en mayúsculas
MI_CONSTANTE_FAVORITA=58.3
