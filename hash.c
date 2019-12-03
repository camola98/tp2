#define _GNU_SOURCE 1
#include "hash.h"
#include "lista.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#define TAM_INICIAL 17
#define CRIT_AGRANDAR 3
#define CRIT_ACHICAR 2
#define FACTOR_CARGA_AMPLIACION 2
#define FACTOR_CARGA_REDUCCION 4

struct hash{
    lista_t** listas;
    size_t cantidad;
    size_t capacidad;
    void (*hash_destruir_dato_t)(void *);
};

typedef struct campo{
    char* clave;
    void* valor;
} campo_t;

/* Busca la próxima posición con una lista no vacía.
 * Si el valor devuelto es igual a la capacidad del hash, 
 * entonces no hay más listas por recorrer */
size_t encontrar_prox_lista(const hash_t* hash, size_t n){
    size_t i = n;

    while (i < hash->capacidad && (!hash->listas[i] || lista_esta_vacia(hash->listas[i]))){
        i++;
    }

    return i;
}

//Función de hash
size_t f_hash(size_t capacidad, const char *str){
    size_t valor = 5381;
    int c;
    while ((c = *str++)){
        valor = ((valor << 5) + valor) + c;
    }

    return valor % capacidad;
}

hash_t *hash_crear(hash_destruir_dato_t destruir_dato){
    hash_t* hash = malloc(sizeof(hash_t));
    if (!hash) return NULL;
    
    hash->listas = calloc(TAM_INICIAL, sizeof(lista_t*));
    if (!hash->listas){
        free(hash);
        return NULL;
    }

    hash->cantidad = 0;
    hash->capacidad = TAM_INICIAL;
    hash->hash_destruir_dato_t = destruir_dato;
    return hash;
}

lista_iter_t* iter_buscar_clave(lista_t* lista, const char* clave){
    lista_iter_t* iter = lista_iter_crear(lista);
    if (!iter) return NULL; 

    while (!lista_iter_al_final(iter)){
        char* clave_act = ((campo_t*)lista_iter_ver_actual(iter))->clave;
        if (!strcmp(clave_act, clave)) break;
        lista_iter_avanzar(iter);
    }

    return iter;
}

campo_t* buscar_campo(const hash_t* hash, const char* clave){
    size_t i = f_hash(hash->capacidad, clave);

    if (!hash->cantidad || !hash->listas[i]) return NULL;

    lista_iter_t* iter_clave = iter_buscar_clave(hash->listas[i], clave); 
    if (!iter_clave) return NULL;
    campo_t* campo = (campo_t*)lista_iter_ver_actual(iter_clave);
    lista_iter_destruir(iter_clave);

    return campo;
}

/* Función que destruye un arreglo de listas enlazadas
 * Si el 2do parámetro es 0, solo destruye las listas. 
 * Si es 1, también destruye las claves y campos. */
void destruir_listas(hash_t* hash, int destruccion_campos){
    for (size_t i = 0; i < hash->capacidad; i++){
        lista_t* lista = hash->listas[i];
        while (destruccion_campos && lista && !lista_esta_vacia(lista)){
            campo_t* campo = lista_borrar_primero(lista);
            if (hash->hash_destruir_dato_t) hash->hash_destruir_dato_t(campo->valor);
            free(campo->clave); free(campo);
        }
        if(lista) lista_destruir(lista, NULL);
    }
    free(hash->listas);
}

bool redimensionar(hash_t* hash, size_t capacidad_nueva){
    lista_t** datos_nuevos = calloc(capacidad_nueva, sizeof(lista_t*));
    if (!datos_nuevos) return false;
    for (size_t i = 0; i < hash->capacidad; i++){
        if (!hash->listas[i]) continue;
        lista_iter_t* lista_iter = lista_iter_crear(hash->listas[i]);
        while (!lista_iter_al_final(lista_iter)){
            campo_t* campo = lista_iter_ver_actual(lista_iter);
            size_t j = f_hash(capacidad_nueva, campo->clave);
            if (!datos_nuevos[j]) datos_nuevos[j] = lista_crear();
            if (!datos_nuevos[j] || !lista_insertar_ultimo(datos_nuevos[j], campo)){
                free(datos_nuevos);
                lista_iter_destruir(lista_iter);
                return false;
            }
            lista_iter_avanzar(lista_iter);
        }
        lista_iter_destruir(lista_iter);
    }
    destruir_listas(hash, 0);
    hash->capacidad = capacidad_nueva;
    hash->listas = datos_nuevos;
    return true;
}

campo_t* generar_campo(const char* clave, void* dato){
    campo_t* campo = malloc(sizeof(campo_t));
    char* _clave = strdup(clave);
    if (!campo || !_clave){
        free(campo); free(_clave);
        return NULL;
    }
    campo->clave = _clave;
    campo->valor = dato;
    return campo;
}

bool hash_guardar(hash_t *hash, const char *clave, void *dato){
    if (hash->cantidad >= (hash->capacidad * FACTOR_CARGA_AMPLIACION)){
        if (!redimensionar(hash, hash->capacidad * CRIT_AGRANDAR)) return false;
    }
    size_t i = f_hash(hash->capacidad, clave);
    if (!hash->listas[i]) hash->listas[i] = lista_crear();
    if (!hash->listas[i]) return false;
    lista_iter_t* iterador = iter_buscar_clave(hash->listas[i], clave);
    if (!iterador) return false;
    if (!lista_iter_al_final(iterador)){
        campo_t* campo = lista_iter_ver_actual(iterador);
        if (hash->hash_destruir_dato_t) hash->hash_destruir_dato_t(campo->valor);
        campo->valor = dato;
    }
    else{
        campo_t* campo = generar_campo(clave, dato);
        if (!campo){
            lista_iter_destruir(iterador);
            return false;
        }
        lista_iter_insertar(iterador, campo);  
        hash->cantidad++;  
    }
    lista_iter_destruir(iterador);
    return true;
}

void *hash_borrar(hash_t *hash, const char *clave){
    size_t i = f_hash(hash->capacidad, clave);

    if (!hash->cantidad || !hash->listas[i]) return NULL;

    lista_iter_t* iter_clave = iter_buscar_clave(hash->listas[i], clave);
    if (!iter_clave || lista_iter_al_final(iter_clave)){
        if (iter_clave) lista_iter_destruir(iter_clave);
        return NULL;
    }
    campo_t* campo = (campo_t*)lista_iter_ver_actual(iter_clave);
    void* valor = campo->valor;
    free(campo->clave);
    free(lista_iter_borrar(iter_clave));
    lista_iter_destruir(iter_clave);
    hash->cantidad--;

    if (hash->cantidad <= (hash->capacidad/FACTOR_CARGA_REDUCCION) && hash->cantidad > TAM_INICIAL){
        redimensionar(hash, hash->capacidad/CRIT_ACHICAR);
    }

    return valor;
}

void *hash_obtener(const hash_t *hash, const char *clave){
    campo_t* campo = buscar_campo(hash, clave);

    if (!campo) return NULL;

    return campo->valor;
}

bool hash_pertenece(const hash_t *hash, const char *clave){
    campo_t* campo = buscar_campo(hash, clave);

    if (!campo) return false;

    return true;
}

size_t hash_cantidad(const hash_t *hash){
    return hash->cantidad;
}

void hash_destruir(hash_t *hash){
    destruir_listas(hash, 1);
    free(hash);
}

struct hash_iter{
    size_t pos;
    size_t cant_iterados;
    lista_iter_t* iter_lista;
    const hash_t* hash;
};

hash_iter_t *hash_iter_crear(const hash_t *hash){
    hash_iter_t* iter = malloc(sizeof(hash_iter_t));
    if (!iter) return NULL;

    if (!hash->cantidad){
        iter->iter_lista = NULL;
        iter->pos = hash->capacidad;
    }
    else{
        size_t i = encontrar_prox_lista(hash, 0);

        lista_iter_t* iter_lista = lista_iter_crear(hash->listas[i]);
        if (!iter_lista){
            free(iter);
            return NULL;
        }
        iter->iter_lista = iter_lista;
        iter->pos = i;
    }
    iter->cant_iterados = 0;
    iter->hash = hash;

    return iter;
}

bool hash_iter_avanzar(hash_iter_t *iter){
    if (hash_iter_al_final(iter)) return false;
    
    lista_iter_avanzar(iter->iter_lista);
    if (lista_iter_al_final(iter->iter_lista) && (iter->cant_iterados+1 != iter->hash->cantidad)){
        size_t i = encontrar_prox_lista(iter->hash, iter->pos + 1);

        lista_iter_t* iter_lista = lista_iter_crear(iter->hash->listas[i]);
        if (!iter_lista) return false;

        lista_iter_destruir(iter->iter_lista);
        iter->pos = i;
        iter->iter_lista = iter_lista;
    }
    iter->cant_iterados++;
    return true;
}

const char *hash_iter_ver_actual(const hash_iter_t *iter){
    if (hash_iter_al_final(iter)) return NULL; 
    campo_t* campo =  (campo_t*)lista_iter_ver_actual(iter->iter_lista);
    return campo->clave;
}

bool hash_iter_al_final(const hash_iter_t *iter){
    return iter->cant_iterados == iter->hash->cantidad;
}

void hash_iter_destruir(hash_iter_t* iter){
    lista_iter_destruir(iter->iter_lista);
    free(iter);
}