#ifndef RoJoTimerRPi_cpp
#define RoJoTimerRPi_cpp

#include <RoJoTimerRPi.h>

void (*_RoJoTimerRPi_iFunctionUser)(); //Puntero a función de interrupción de usuario
bool _RoJoTimerRPi_running=false; //El timer está en ejecución?
bool _RoJoTimerRPi_once; //El timer es de una sóla ejecución?

//Función de interrupción interna
void _RoJoTimerRPi_iFunctionInternal(int signalType) {
  _RoJoTimerRPi_iFunctionUser(); //Llamamos a la función de interrupción de usuario
  if(_RoJoTimerRPi_once) _RoJoTimerRPi_running=false; //Si sólo se ejecuta una vez, ya no está en ejecución
}

//Activa el timer indefinidamente
//Parámetros:
//  - period_s : periodo en segundos
//  - callback : función de interrupción
void RoJoTimerRPi::attach(float period_s,void (*callback)()) {
  //Periodo de repetición
  _timerConfig.it_interval.tv_sec=int(period_s); //Segundos
  _timerConfig.it_interval.tv_usec=int((period_s-int(period_s))*1000000-0); //Microsegundos
  //Tiempo de espera para primera ejecución
  _timerConfig.it_value.tv_sec=0; //Segundos
  _timerConfig.it_value.tv_usec=1; //Microsegundos. Ejecución inmediata
  _RoJoTimerRPi_iFunctionUser=callback; //Guardamos la función de interrupción de usuario
  _RoJoTimerRPi_once=false; //No es un timer de una sola ejecución
  //Asignamos la función de interrupción interna al tipo de señal
  signal(SIGALRM,_RoJoTimerRPi_iFunctionInternal);
  _RoJoTimerRPi_running=true; //En ejecución
  setitimer(ITIMER_REAL,&_timerConfig,NULL); //Aplicamos la configuración al timer
}

//Activa el timer indefinidamente
//Parámetros:
//  - period_s : periodo en milisegundos
//  - callback : función de interrupción
void RoJoTimerRPi::attach_ms(uint32_t period_ms, void (*callback)()) {
  attach((float)period_ms/1000.0,callback);
}

//Activa el timer una vez
//Parámetros:
//  - period_s : periodo en segundos
//  - callback : función de interrupción
void RoJoTimerRPi::once(float period_s, void (*callback)()) {
  //Periodo de repetición
  _timerConfig.it_interval.tv_sec=_timerConfig.it_interval.tv_usec=0; //No se repite
  //Tiempo de espera para primera ejecución
  _timerConfig.it_value.tv_sec=int(period_s); //Segundos
  _timerConfig.it_value.tv_usec=int((period_s-int(period_s))*1000000-0); //Microsegundos
  _RoJoTimerRPi_iFunctionUser=callback; //Guardamos la función de interrupción de usuario
  _RoJoTimerRPi_once=true; //Es un timer de una sola ejecución
  //Asignamos la función de interrupción interna al tipo de señal
  signal(SIGALRM,_RoJoTimerRPi_iFunctionInternal);
  _RoJoTimerRPi_running=true; //En ejecución
  setitimer(ITIMER_REAL,&_timerConfig,NULL); //Aplicamos la configuración al timer
}

//Activa el timer una vez
//Parámetros:
//  - period_s : periodo en milisegundos
//  - callback : función de interrupción
void RoJoTimerRPi::once_ms(uint32_t period_ms, void (*callback)()) {
  once((float)period_ms/1000.0,callback);
}

//Deshabilita la interrupción
void RoJoTimerRPi::detach() {
	_timerConfig.it_value.tv_sec=_timerConfig.it_value.tv_usec=0; //No comenzará
	setitimer(ITIMER_REAL,&_timerConfig,NULL);
  _RoJoTimerRPi_running=false; //Ya no está en ejecución
}

//El timer está activo?
bool RoJoTimerRPi::active() {
  return _RoJoTimerRPi_running;
}

#endif
