#En ESP-IDF el archivo principal es main.c no main.cpp
#Para guardar compatibilidad indicaremos explícitamente el archivo principal.
#main.c simplemente llamará a main.cpp 
FILE(GLOB_RECURSE app_sources ${CMAKE_SOURCE_DIR}/src/main.c)
idf_component_register(SRCS ${app_sources})