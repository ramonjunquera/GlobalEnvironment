/*
  Nombre de la librería: RoJoTimerRPi.h
  Versión: 20221027
  Autor: Ramón Junquera
  Notas:
  - La función de interrupción no tiene parámetros ni devuelve nada
  - No se deberían crear varias instancias de esta clase, porque sólo hay un único hardware para
    la interrupción.
  - Internamente la función de interrupción debe tener un parámetro de tipo int.
    Para guardar compatibilidad con otras librerías de gestión de interrupciones, creamos una función
    de interrupción interna con parámetro que simplemente llama a la función de interrupción definida
    por el usuario que no tiene parámetros.
    Esta función se guarda en una variable global (un puntero). No se puede incluir dentro de la clase.
  - La configuración de la interrupción divide el tiempo en dos variables de tipo uint32_t.
    Una guarda los segundos y otra los microsegundos. El tiempo total es la suma de ambas.
    Si igualamos ambas a 0 indicaremos que no queremos que salte.
    El tiempo máximo que se puede almacenar es de (1^32-1) segundos + (1^32-1) microsegundos.
    Aproximadamente unos 136,2 años.
  - Existen dos secciones para asignar tiempos:
    - Tiempo que debe transcurrir hasta que se lance la interrupción por primera vez
    - Tiempo que debe transcurrir para que se repita la interrupción
*/

#ifndef RoJoTimerRPi_h
#define RoJoTimerRPi_h

#include <Arduino.h>
#include <sys/time.h> //para itimerval
#include <signal.h> //para signal

class RoJoTimerRPi {
  private:  //Definición de métodos/variables privadas
    struct itimerval _timerConfig; //Configuración
  public: //Definición de métodos/variables públicas
    void detach(); //Detiene el timer
    void attach(float period_s,void (*callback)()); //Activa el timer indefinidamente
    void attach_ms(uint32_t period_ms, void (*callback)()); //Activa el timer indefinidamente
    void once(float period_s, void (*callback)()); //Activa el timer una vez
    void once_ms(uint32_t period_ms, void (*callback)()); //Activa el timer una vez
    bool active(); //El timer está activo?
}; //Punto y coma obligatorio para que no de error

#include <RoJoTimerRPi.cpp>

#endif
