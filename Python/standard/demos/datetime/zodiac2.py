from datetime import datetime

monthName=("Enero","Febrero","Marzo","Abril","Mayo","Junio","Julio","Agosto","Septiembre","Octubre","Noviembre","Diciembre") #Tupla
dayName=("Lunes","Martes","Miércoles","Jueves","Viernes","Sábado","Domingo")
zodiac=( #Fecha de inicio de cada uno de los signos zodiacales en el año 2024. Tomamos este año porque es bisiesto
  (datetime(2024,1,20),"Acuario"),
  (datetime(2024,2,19),"Piscis"),
  (datetime(2024,3,21),"Aries"),
  (datetime(2024,4,20),"Tauro"),
  (datetime(2024,5,21),"Géminis"),
  (datetime(2024,6,21),"Cáncer"),
  (datetime(2024,7,23),"Leo"),
  (datetime(2024,8,23),"Virgo"),
  (datetime(2024,9,23),"Libra"),
  (datetime(2024,10,23),"Escorpio"),
  (datetime(2024,11,22),"Sagitario"),
  (datetime(2024,12,22),"Capricornio")
)

#Compone un texto con la fecha completa
def dateString(date):
  return "{} de {} de {}".format(date.day,monthName[date.month-1],date.year)

birthday=0
while(birthday==0):
  try:
    birthdayString=input("Introduce tu fecha de nacimiento en formato dd/mm/aaaa: ")
    birthday=datetime.strptime(birthdayString,"%d/%m/%Y")
  except ValueError:
    birthday=0

print("Naciste el {}".format(dateString(birthday)))
print("Ese día fue {}".format(dayName[birthday.weekday()]))

now=datetime.today()
now=datetime(now.year,now.month,now.day) #Quitamos las horas
print("Desde entonces han transcurrido {} días".format((now-birthday).days))

yearsNow=now.year-birthday.year #Años que cumple este año
birthdayNow=0 #Fecha de cumpleaños en año actual
try:
  birthdayNow=datetime(now.year,birthday.month,birthday.day)
  #Si la operación falla es debido a que nació un año bisiesto, un 29 de febrero y el año actual no es bisiesto
  #Por lo tanto no puese encontrar el 29 de febrero
except ValueError:
  #En este caso, supondremos que los años no bisiestos celebra su cumpleaños el 1 de marzo
  birthdayNow=datetime(now.year,3,1)
if(now>birthdayNow): #Si ya ha pasado la fecha del cumpleaños este año...
  print("Hace {} días que cumpliste {} años".format((now-birthdayNow).days,yearsNow))
elif (now<birthdayNow): #Si aun no ha llegado la fecha del cumpleaños...
  print("Cumpliste {} años hace {} días".format(yearsNow,(birthdayNow-now).days,yearsNow))
else: #Si hoy es el cumpleaños...
  print("Hoy cumples {} años".format(yearsNow))

birthday2024=datetime(2024,birthday.month,birthday.day) #Fecha de cumpleaños en año 2024
zodIndex=11 #Índice de signo zodiacal. Inicialmente Capricornio
for i in range(0,11):
  if(birthday2024>=zodiac[i][0] and birthday2024<zodiac[i+1][0]):
    zodIndex=i
    break
print("Tu signo zodiacal es {}".format(zodiac[zodIndex][1]))
