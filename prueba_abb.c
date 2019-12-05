#include "abb.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

bool printear_todos(const char *clave, void *dato, void* extra){
    if (*(int*)extra >= 5) return false;
    *(int*)extra += 1;
    printf("%s\n", clave);
    return true;
}

void pruebas_iterador_interno(){
    
    printf("\nINICIO DE PRUEBAS ITERADOR INTERNO\n");

    /* Declaro las variables a utilizar*/
    abb_t* arbol = abb_crear(strcmp, NULL);
    int contador = 0; 

    char *claves[] = {"b", "u", "s", "a", "t", "c", "x", "d", "g", "h", "m", "w", "r", "l"};

     /* Inicio de pruebas guardando los elementos sin usar iterador */
    abb_guardar(arbol, claves[0], NULL);
    abb_guardar(arbol, claves[1], NULL);
    abb_guardar(arbol, claves[2], NULL);
    abb_guardar(arbol, claves[3], NULL);
    abb_guardar(arbol, claves[4], NULL);
    abb_guardar(arbol, claves[5], NULL);
    abb_guardar(arbol, claves[6], NULL);
    abb_guardar(arbol, claves[7], NULL);
    abb_guardar(arbol, claves[8], NULL);
    abb_guardar(arbol, claves[9], NULL);
    abb_guardar(arbol, claves[10], NULL);
    abb_guardar(arbol, claves[11], NULL);
    abb_guardar(arbol, claves[12], NULL);
    abb_guardar(arbol, claves[13], NULL);
    
    abb_visitar_rangos(arbol, "l", "z", printear_todos, &contador);

    abb_destruir(arbol);
}

int main(){
    pruebas_iterador_interno();
    return 0;
}