#define _GNU_SOURCE 1
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include "abb.h"
#include "heap.h"
#include "hash.h"
#include "lista.h"
#include "strutil.h"

typedef enum recorrido{ASC, DESC, ERROR} recorrido_t;
typedef enum comando{AGREGAR_ARCHIVO, VER_TABLERO, INFO_VUELO, PRIORIDAD_VUELOS, BORRAR, INVALIDO} comando_t;

typedef struct adm_vuelos{
    hash_t* codigos;
    abb_t* fechas_despegues;
} adm_vuelos_t;

typedef struct tablero{
    int k;
    int contador;    
} tablero_t;

int comparar_vuelos(const char* vuelo_a, const char* vuelo_b){
    char* fecha_a = substr(vuelo_a, 19);
    char* fecha_b = substr(vuelo_b, 19);
    int n = strcmp(fecha_a, fecha_b);
    if (n == 0) n = strcmp(vuelo_a+20, vuelo_b+20);
    free(fecha_a); free(fecha_b);
    return n;
}

void free_para_hash(void* dato){
    char** arreglo = dato;
    free_strv(arreglo);
}

adm_vuelos_t* adm_vuelos_crear(){
    adm_vuelos_t* vuelos = malloc(sizeof(adm_vuelos_t));
    hash_t* hash = hash_crear(free_para_hash);
    abb_t* abb = abb_crear(comparar_vuelos, NULL);
    if (!vuelos || !hash || !abb) return NULL;
    vuelos->codigos = hash;
    vuelos->fechas_despegues = abb;
    return vuelos;
}

int comparar_prioridad(const void* a, const void* b){
     char** _a = split((char*)a, '-');
     char** _b = split((char*)b, '-');
     int n;
     int a0 = atoi(_a[0]), b0 = atoi(_b[0]);
     if (a0 < b0) n = -1;
     else if (a0 > b0) n = 1;
     else n = strcmp(_b[1], _a[1]);
     free_strv(_a); free_strv(_b);
     return n;
}

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

bool agregar_archivo(adm_vuelos_t* adm_vuelos, char** info, int cant_parametros){
    if (cant_parametros < 2) return false;
    char* file_name = info[1];
    FILE* file = fopen(file_name, "r");
    if (!file) return false;
    char* linea = NULL;
    size_t capacidad = 0;
    while (getline(&linea, &capacidad, file) != -1){
        char** arreglo = split(linea, ',');
        if (hash_pertenece(adm_vuelos->codigos, arreglo[0])){
            char* clave_abb = unir_str(((char**)hash_obtener(adm_vuelos->codigos, arreglo[0]))[6], arreglo[0]);
            abb_borrar(adm_vuelos->fechas_despegues, clave_abb);
            free(clave_abb);
        } 
        if (!hash_guardar(adm_vuelos->codigos, arreglo[0], arreglo)){fclose(file); return false;}
        char* str_unido = unir_str(arreglo[6], arreglo[0]);
        if (!abb_guardar(adm_vuelos->fechas_despegues, str_unido, NULL)){
            fclose(file); free(str_unido); free_strv(hash_borrar(adm_vuelos->codigos, arreglo[0])); return false;
        }
        free(str_unido);
    }
    free(linea);
    fclose(file);
    return true;
}

recorrido_t identificar_recorrido(char* recorrido){
    if (!strcmp(recorrido, "asc")) return ASC;
    if (!strcmp(recorrido, "desc")) return DESC;
    return ERROR;
}

bool verificar_parametros(int k, recorrido_t recorrido, char* desde, char* hasta){
    if (k <= 0) return false;
    if (recorrido == ERROR) return false;
    if (strcmp(hasta, desde) < 0) return false;
    return true;
}

bool recorrer_vuelos(const char *clave, void *dato, void* extra){
    tablero_t* tablero = extra;
    if (tablero->contador >= tablero->k) return false;
    tablero->contador += 1;
    fprintf(stdout, "%s\n", clave);
    return true;
}

tablero_t* crear_aux_iterador_tablero(int k){
    tablero_t* tablero = malloc(sizeof(tablero_t));
    if (!tablero) return NULL;
    tablero->k = k;
    tablero->contador = 0;
    return tablero;
}

bool ver_tablero(adm_vuelos_t* adm_vuelos, char** info, int cant_parametros){
    if (cant_parametros < 5) return false;
    int k = atoi(info[1]);
    recorrido_t _recorrido = identificar_recorrido(info[2]);
    char* desde = info[3];
    char* hasta = info[4];
    if (!verificar_parametros(k, _recorrido, desde, hasta)) return false;
    tablero_t* aux_iterador_tablero = crear_aux_iterador_tablero(k);
    if (!aux_iterador_tablero) return false;
    if (_recorrido == ASC) abb_visitar_rangos(adm_vuelos->fechas_despegues, desde, hasta, recorrer_vuelos, aux_iterador_tablero, 0);
    if (_recorrido == DESC) abb_visitar_rangos(adm_vuelos->fechas_despegues, desde, hasta, recorrer_vuelos, aux_iterador_tablero, 1);
    free(aux_iterador_tablero);
    return true;
}

bool info_vuelo(adm_vuelos_t* adm_vuelos, char** info, int cant_parametros){
    if (cant_parametros < 2) return false;
    char* nro_vuelo = info[1];
    char** vuelo = hash_obtener(adm_vuelos->codigos, nro_vuelo);
    if (!vuelo) return false;
    char* info_a_mostrar = join(vuelo, ' ');
    fprintf(stdout, "%s", info_a_mostrar);
    free(info_a_mostrar);
    return true;    
}

bool hash_encolar_todo(heap_t* heap, hash_t* hash){
    hash_iter_t* hash_iter = hash_iter_crear(hash);
    if (!hash_iter) return false;
    while (!hash_iter_al_final(hash_iter)){
        char* actual = strdup(hash_iter_ver_actual(hash_iter));
        char* str_unido = unir_str(((char**)hash_obtener(hash, actual))[5], actual);
        heap_encolar(heap, str_unido);
        free(actual);
        hash_iter_avanzar(hash_iter);
    }
    hash_iter_destruir(hash_iter);
    return true;
}

bool prioridad_vuelos(adm_vuelos_t* adm_vuelos, char** info, int cant_parametros){
    if (cant_parametros < 2) return false;
    int k = atoi(info[1]);
    if (k <= 0) return false;
    heap_t* vuelos = heap_crear(comparar_prioridad);
    if (!vuelos) return false;
    if (!hash_encolar_todo(vuelos, adm_vuelos->codigos));
    for (int i = 0; i < k && i < hash_cantidad(adm_vuelos->codigos); i++){
        char* vuelo = heap_desencolar(vuelos);
        fprintf(stdout, "%s\n", vuelo);
        free(vuelo);
    }
    heap_destruir(vuelos, free);
    return true;
}

bool borrar_rango(const char* clave, void* dato, void* extra){
    lista_t* lista_aux = extra;
    lista_insertar_ultimo(lista_aux, strdup(clave));
    return true;
}

bool borrar(adm_vuelos_t* adm_vuelos,char** info, int cant_parametros){
    if (cant_parametros < 3) return false;
    char* desde = info[1];
    char* hasta = info[2];
    if (!verificar_parametros(1, ASC, desde, hasta)) return false;
    lista_t* lista_aux = lista_crear();
    if (!lista_aux) return false;
    abb_visitar_rangos(adm_vuelos->fechas_despegues, desde, hasta, borrar_rango, lista_aux, 0);
    while (!lista_esta_vacia(lista_aux)){
        char* clave_a_borrar = lista_borrar_primero(lista_aux);
        abb_borrar(adm_vuelos->fechas_despegues, clave_a_borrar);
        char** vuelo_arr = hash_borrar(adm_vuelos->codigos, clave_a_borrar+22);
        char* vuelo_join = join(vuelo_arr, ' ');
        fprintf(stdout, "%s", vuelo_join);
        free_strv(vuelo_arr); free(vuelo_join); free(clave_a_borrar);
    }
    lista_destruir(lista_aux, NULL);
    return true;
}

bool ejecutar_comando(adm_vuelos_t* adm_vuelos, comando_t comando, char** info, int cant_parametros){
    if (comando == INVALIDO) return false;
    if (comando == AGREGAR_ARCHIVO && !agregar_archivo(adm_vuelos, info, cant_parametros)) return false;
    if (comando == VER_TABLERO && !ver_tablero(adm_vuelos, info, cant_parametros)) return false;
    if (comando == INFO_VUELO && !info_vuelo(adm_vuelos, info, cant_parametros)) return false;
    if (comando == PRIORIDAD_VUELOS && !prioridad_vuelos(adm_vuelos, info, cant_parametros)) return false;
    if (comando == BORRAR && !borrar(adm_vuelos, info, cant_parametros)) return false;
    return true;
}

char** splitear_linea(char* linea){
    size_t ultimo_char = strlen(linea) - 1;

    if (linea[ultimo_char] == '\n') linea[ultimo_char] = '\0';
    char* sin_salto_linea = strdup(linea);
    if (!sin_salto_linea) return NULL;

    char** valores_linea = split(sin_salto_linea, ' ');
    free(sin_salto_linea); 
    if (!valores_linea) return NULL;

    return valores_linea;
}

void algueiza(adm_vuelos_t* adm_vuelos){
    char* linea = NULL;
    size_t len = 0;
    while(getline(&linea, &len, stdin) != -1){
        char** valores_linea = splitear_linea(linea);
        int cant_parametros = 0;
        for (int i = 0; valores_linea[i]; i++) cant_parametros += 1;
        if (!valores_linea) fprintf(stderr, "Error en programa\n");
        comando_t comando = identificar_comando(valores_linea[0]);
        if (!ejecutar_comando(adm_vuelos, comando, valores_linea, cant_parametros)) fprintf(stderr, "Error en comando %s\n", valores_linea[0]);
        else fprintf(stdout, "OK\n");
        free_strv(valores_linea);
    }
    free(linea);
}

int main(){
    adm_vuelos_t* adm_vuelos = adm_vuelos_crear();
    if (!adm_vuelos){
        fprintf(stderr, "Error en programa\n");
        return 1;
    }
    algueiza(adm_vuelos);
    hash_destruir(adm_vuelos->codigos);
    abb_destruir(adm_vuelos->fechas_despegues);
    free(adm_vuelos);
    return 0;
}