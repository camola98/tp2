#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include "abb.h"
#include "heap.h"
#include "hash.h"
#include "strutil.h"

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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef struct vuelo{
       char* codigo;
       char* aerolinea;
       char* aeropuerto_origen;
       char* aeropuerto_destino;
       char* tail_number;
       size_t prioridad;
       char* tiempo;
       size_t* air_time;
       size_t* cancelled;
}vuelo_t;

vuelo_t* vuelo_crear(char** info){
    vuelo_t* flight = malloc(sizeof(vuelo_t));
    if (!flight) return NULL;
    flight->codigo = info[0];
    flight->aerolinea = info[1];
    flight->aeropuerto_origen = info[2];
    flight->aeropuerto_destino = info[3];
    flight->tail_number = info[4];
    flight->prioridad = *(size_t*)info[5];
    flight->tiempo = info[6];
    flight->air_time = *(size_t*)info[7];
    flight->cancelled = *(size_t*)info[8];
    return flight;
}

typedef struct adm_vuelos{
    hash_t* codigos_vuelos;
    heap_t* prioridad_vuelos;
    abb_t* arbol_rangos;
}adm_vuelos_t*;

adm_vuelos_t* adm_vuelos_crear(){
    adm_vuelos_t* flights = malloc(sizeof(adm_vuelos_t));
    hash_t* hash = hash_crear();
    heap_t* heap = heap_crear();
    abb_t* abb = abb_crear();
    if (!flights || !hash || !heap || !abb) return NULL;
    flights->codigos_vuelos = hash;
    flights->prioridad_vuelos = heap;
    flights->arbol_rangos = abb;
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
vuelo_t** lector_archivo(char* file_name, char* comando ){
    FILE* file = fopen(file_name, "r");
    if (!file){
        fprintf( stderr, "Error en comando < %s >", comando);
        return;
        }
    char* linea = NULL;
    size_t capacidad = 0;
    while (getline(&linea, &capacidad, file) != -1){
        char** arreglo = split(linea, ',');
        vuelo_t* vuelo = vuelo_crear(arreglo);
    }
}
