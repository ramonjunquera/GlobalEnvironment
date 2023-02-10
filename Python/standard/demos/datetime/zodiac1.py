import datetime

mes=0
resOk=False #Es válida la respuesta?

print("Cuándo naciste?")

#Solicitamos el mes hasta que tengamos un valor válido
while not resOk:
  try:
    mes=int(input("mes [1,12]? "))
  except ValueError:
    mes=0
  if(mes>=1 and mes <=12): resOk=True
#Hemos conseguido un més válido

daysPerMonth=(31,29,31,30,31,30,31,31,30,31,30,31) #Tupla de número de días por mes
dia=0
resOk=False
#Solicitamos un día hasta que tengamos un valor válido
while not resOk:
  try:
    dia=int(input("dia? "))
  except ValueError:
    dia=0
  if(dia>=1 and dia<=daysPerMonth[mes-1]): resOk=True
#Hemos conseguido un día válido

monthName=("Enero","Febrero","Marzo","Abril","Mayo","Junio","Julio","Agosto","Septiembre","Octubre","Noviembre","Diciembre") #Tupla
print("Naciste el {} de {}".format(dia,monthName[mes-1]))

#Creamos una variable de tipo datetime con los elementos obtenidos
nacimiento=datetime.datetime(2024,mes,dia) #2024 porque es bisiesto y aceptará el 29 de Febrero

#Tupla de fecha de inicio y nombre de signo zodiacal
zodiac=(
  (datetime.datetime(2024,1,20),"Acuario"),
  (datetime.datetime(2024,2,19),"Piscis"),
  (datetime.datetime(2024,3,21),"Aries"),
  (datetime.datetime(2024,4,20),"Tauro"),
  (datetime.datetime(2024,5,21),"Géminis"),
  (datetime.datetime(2024,6,21),"Cáncer"),
  (datetime.datetime(2024,7,23),"Leo"),
  (datetime.datetime(2024,8,23),"Virgo"),
  (datetime.datetime(2024,9,23),"Libra"),
  (datetime.datetime(2024,10,23),"Escorpio"),
  (datetime.datetime(2024,11,22),"Sagitario"),
  (datetime.datetime(2024,12,22),"Capricornio")
)

zodIndex=11 #Capricornio
for i in range(0,11):
  if(nacimiento>=zodiac[i][0] and nacimiento<zodiac[i+1][0]):
    zodIndex=i
    break
print("Signo zodiacal: {}".format(zodiac[zodIndex][1]))

