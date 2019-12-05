#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include "abb.h"
#include "pila.h"
#include "heap.h"
#include "hash.h"
#include "strutil.h"

typedef enum recorrido{ASC, DESC} recorrido_t;
typedef enum comando{AGREGAR_ARCHIVO, VER_TABLERO, INFO_VUELO, PRIORIDAD_VUELOS, BORRAR, INVALIDO} comando_t;

/* tp 2 */
/* 
Interfaz comandos:

agregar_archivo <nombre_archivo>: 
procesa de forma completa un archivo de .csv que contiene datos de vuelos.

ver_tablero <K cantidad vuelos> <modo: asc/desc> <desde> <hasta>: 
muestra los K vuelos ordenados por fecha de forma ascendente (asc) o 
descendente (desc), cuya fecha de despegue esté dentro de el intervalo 
<desde> <hasta> (inclusive).

info_vuelo <código vuelo>: 
muestra toda la información posible en sobre el vuelo que tiene el código 
pasado por parámetro.

prioridad_vuelos <K cantidad vuelos>: 
muestra los códigos de los K vuelos que tienen mayor prioridad.

borrar <desde> <hasta>: 
borra todos los vuelos cuya fecha de despegue estén dentro del intervalo 
<desde> <hasta> (inclusive).

Si un comando es válido deberá imprimir OK por salida estándar después 
de ser ejecutado. Si un comando no pertenece a los listados previamente o 
tiene un error, se imprime Error en comando <comando> por stderr y 
continua la ejecución.
*/

typedef struct tablero{
    recorrido_t recorrido;
    pila_t* pila;
    size_t k;
    size_t contador;    
}

//Función aux para comparar vuelos por fecha. Ej.: '2018-10-10T08:51:32 - 1234'
size_t comparar_vuelos(char* vuelo_a, char* vuelo_b){
    const char* fecha_a = substr(vuelo_a, 19);
    const char* fecha_b = substr(vuelo_b, 19);
    int n = strcmp(fecha_a, fecha_b);
    if (n == 0){
        const char* codigo_a, codigo_b;
        strcat(codigo_a, vuelo_a+20);
        strcat(codigo_b, vuelo_b+20);
        n = strcmp(codigo_a, codigo_b);
    }
    return n;
}

// Función aux para comparar priordad de dos vuelos
int comparar_prioridad(char** arreglo, char* valor){
    return strcmp(arreglo[5], valor);
    //if (a < b) devuelve -1;
    //if (a > b) devuelve 1;
}

heap_t* heap_de_prioridades(hash_t* hash, cmp_func_t cmp, size_t cantidad){
    //heap de minimos
    heap_t* heap = heap_crear(cmp);
    hash_iter_t* iter = hash_iter_crear(hash);
    if (!heap) return NULL;
    while (heap_cantidad(heap)!= cantidad && !hash_iter_al_final(iter)){
        //heap ver max en heap de minimo funciona como heap ver min
        if (cmp(hash_obtener(hash,hash_iter_ver_actual(iter)), heap_ver_max(heap))>0){
            if (heap_cantidad(heap) == cantidad) heap_desencolar(heap);
            heap_encolar(heap, hash_iter_ver_actual(iter));
        }
        hash_iter_avanzar(iter);
    }
return heap;
}


// typedef struct vuelo{
//        char* codigo;
//        char* aerolinea;
//        char* aeropuerto_origen;
//        char* aeropuerto_destino;
//        char* numero_cola;
//        size_t prioridad;
//        char* tiempo;
//        char* tiempo_vuelo;
//        char* cancelado;
// } vuelo_t;

// vuelo_t* vuelo_crear(char** info){
//     vuelo_t* vuelo = malloc(sizeof(vuelo_t));
//     if (!vuelo) return NULL;
//     vuelo->aerolinea = info[1];
//     vuelo->aeropuerto_origen = info[2];
//     vuelo->aeropuerto_destino = info[3];
//     vuelo->numero_cola = info[4];
//     vuelo->prioridad = atoi(info[5]);
//     vuelo->tiempo = info[6];
//     vuelo->tiempo_vuelo = info[7];
//     vuelo->cancelado = info[8];
//     return vuelo;
// }

typedef struct adm_vuelos{
    hash_t* codigos;
    abb_t* fechas_despegues;
} adm_vuelos_t;

adm_vuelos_t* adm_vuelos_crear(){
    adm_vuelos_t* vuelos = malloc(sizeof(adm_vuelos_t));
    hash_t* hash = hash_crear(free_strv);
    abb_t* abb = abb_crear(comparar_vuelos, NULL);
    if (!vuelos || !hash || !abb) return NULL;
    vuelos->codigos = hash;
    vuelos->fechas_despegues = abb;
    return vuelos;
}

//procesar el csv
/*ej. formato
    FLIGHT_NUMBER: 4608
    AIRLINE: OO
    ORIGIN_AIRPORT: PDX
    DESTINATION_AIRPORT: SEA
    TAIL_NUMBER: N812SK
    PRIORITY: 08
    DATE: 2018-04-10T23:22:55
    DEPARTURE_DELAY: 05
    AIR_TIME: 43
    CANCELLED: 0
*/

comando_t identificar_comando(char* valor){
    if (!strcmp(valor, "agregar_archivo")) return AGREGAR_ARCHIVO;
    if (!strcmp(valor, "ver_tablero")) return VER_TABLERO;
    if (!strcmp(valor, "info_vuelo")) return INFO_VUELO;
    if (!strcmp(valor, "prioridad_vuelos")) return PRIORIDAD_VUELOS;
    if (!strcmp(valor, "borrar")) return BORRAR;
    return INVALIDO;
}

char* unir_str(char* str1, char* str2){
    char* strings[] = {str1, "-", str2, NULL};
    char* str_unidas = join(strings, ' ');
    return str_unidas;
}

bool agregar_archivo(adm_vuelos_t* adm_vuelos, char* file_name){
    FILE* file = fopen(file_name, "r");
    if (!file) return false;
    char* linea = NULL;
    size_t capacidad = 0;
    while (getline(&linea, &capacidad, file) != -1){
        char** arreglo = split(linea, ',');
        if (!hash_guardar(adm_vuelos->codigos, arreglo[0], arreglo)){
            fclose(file); return false;
        }
        char* str_unido = unir_str(arreglo[6], arreglo[0]);
        if (!abb_guardar(adm_vuelos->fechas_despegues, unir_str, NULL)){
            fclose(file); free(str_unido); free_strv(hash_borrar(adm_vuelos->codigos, arreglo[0])); return false;
        }
        free(str_unido);
    }
    free(linea);
    return true;
}

recorrido_t identificar_recorrido(char* recorrido){
    if (!strcmp(recorrido, "asc")) return ASC;
    if (!strcmp(recorrido, "desc")) return DESC;
    return INVALIDO;
}

bool verificar_parametros(size_t k, recorrido_t recorrido, char* desde, char* hasta){
    if (k <= 0) return false;
    if (recorrido == INVALIDO) return false;
    if (strcmp(hasta, desde) < 0) return false;
    return true;
}

bool recorrer_vuelos(const char *clave, void *dato, void* extra){
    if (extra->contador >= extra->k) return false;
    extra->contador += 1;
    if (extra->recorrido == ASC) fprintf(stdout, "%s\n", clave);
    else pila_apilar(extra->pila, clave);
    return true;
}

tablero_t* crear_tablero(recorrido_t recorrido, size_t k){
    tablero_t* tablero = malloc(sizeof(tablero_t));
    if (!tablero) return NULL;
    tablero->k = k;
    tablero->contador = 0;
    tablero->recorrido = recorrido;
    tablero->pila = NULL;
    if (recorrido == DESC){
        tablero->pila = pila_crear();
        if (!tablero->pila){ 
            free(tablero);
            return NULL;
        }
    }
    return tablero;
}

bool ver_tablero(adm_vuelos_t* adm_vuelos, size_t k, char* recorrido, char* desde, char* hasta){
    recorrido_t _recorrido = identificar_recorrido(recorrido);
    if (!verificar_parametros(k, _recorrido, desde, hasta)) return false;
    tablero_t* tablero = crear_tablero(_recorrido, k);
    if (!tablero) return false;
    abb_visitar_rangos(adm_vuelos->fechas_despegues, desde, hasta, recorrer_vuelos, tablero);
    if (_recorrido == DESC){
        while(!pila_esta_vacia(tablero->pila)) fprintf(stdout, "%s", pila_desapilar(tablero->pila));
    }
    return true;
}

heap_t* heap_de_prioridades(hash_iter_t* iter, cmp_func_t cmp, size_t cantidad){
    //heap de maximos
    heap_t* heap = heap_crear(cmp);
    if (!heap) return NULL;
    while (heap_cantidad(heap)!= cantidad && !hash_iter_al_final(iter)){
        if (cmp(hash_iter_ver_actual(iter), heap_ver_max(heap))<0){
            if (heap_cantidad(heap) == cantidad) heap_desencolar(heap);
            heap_encolar(heap, hash_iter_ver_actual(iter));
        }
        hash_iter_avanzar(iter);
    }
    return heap;
}

bool info_vuelo(adm_vuelos_t* adm_vuelos, char* nro_vuelo){
    char** vuelo = hash_obtener(adm_vuelos->codigos, nro_vuelo);
    if (!vuelo) return false;
    char* info = join(vuelo, ' ');
    fprintf(stdout, "%s", info);
    free(info);
    return true;    
}

bool ejecutar_comando(adm_vuelos_t* adm_vuelos, comando_t comando, char** info_extra){
    if (comando == INVALIDO) return false;
    if (comando == AGREGAR_ARCHIVO && !agregar_archivo(adm_vuelos, info_extra[1])) return false;
    if (comando == VER_TABLERO && !ver_tablero(adm_vuelos, info_extra[1])) return false;
    if (comando == INFO_VUELO && !info_vuelo(adm_vuelos, info_extra[1])) return false;
    if (comando == PRIORIDAD_VUELOS && !prioridad_vuelos(adm_vuelos)) return false;
    if (comando == BORRAR && !borrar(adm_vuelos)) return false;
    return true;
}

bool borrar_rango(const char* clave, void* dato, adm_vuelos_t* adm_vuelos){
    if (!abb_borrar(adm_vuelos->fechas_despegues, clave)) return false;
    if (!hash_borrar(adm_vuelos->codigos, clave)) return false;
    return true;
}

bool borrar(adm_vuelos_t* adm_vuelos, size_t cantidad, char* modo, char* desde, char* hasta){
    if (!verificar_parametros(*(size_t*)1, ASC, desde, hasta)) return false;
    if(!visitar_rangos(adm_vuelos->fechas_despegues, desde, hasta, borrar_rango, adm_vuelos)) return false;
    return true;
}

void algueiza(adm_vuelos_t* adm_vuelos){
    char* linea = NULL;
    size_t len = 0;
    while(getline(&linea, &len, stdin) != -1){
        char* copia_cad = strdup(linea);
        char** valores_linea = split(copia_cad, ' ');
        if (!valores_linea) fprintf(stderr, "Error en programa");
        comando_t comando = identificar_comando(valores_linea[0]);
        if (!ejecutar_comando(adm_vuelos, comando, valores_linea)) fprintf(stdout, "Error en comando %s", valores_linea[0]);
        else fprintf(stdout, "OK", valores_linea[0]);
        free_strv(valores_linea);
    }
    free(linea);
}

int main(){
    adm_vuelos_t* adm_vuelos = adm_vuelos_crear();
    if (!adm_vuelos){
        fprintf(stderr, "Error en programa");
        return 1;
    }
    algueiza(adm_vuelos);
    hash_destruir(adm_vuelos->codigos);
    abb_destruir(adm_vuelos->fechas_despegues);
    free(adm_vuelos);
    return 0;
}