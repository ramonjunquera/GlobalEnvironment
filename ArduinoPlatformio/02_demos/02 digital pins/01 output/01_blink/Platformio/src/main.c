//En ESP-IDF el archivo principal es main.c no main.cpp
//Aquí sólo llamaremos a main.cpp que es el verdadero archivo principal
#ifdef ESPIDF
  #include <main.cpp>
#endif
