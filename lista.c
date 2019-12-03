#include "lista.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

typedef struct nodo {
	void* dato;
	struct nodo* prox;
} nodo_t;


struct lista {
	nodo_t* prim;
	nodo_t* ult;
    size_t largo;
};

struct lista_iter {
	lista_t* lista;
	nodo_t* act;
	nodo_t* ant;
};

nodo_t* crear_nodo(void* valor) { 
    nodo_t* nodo = malloc(sizeof(nodo_t)); 
    if (!nodo) return NULL; 
    nodo->dato = valor; 
    nodo->prox = NULL; 
    return nodo; 
}

lista_t *lista_crear(void){
    lista_t* lista = malloc(sizeof(lista_t));
    if (!lista) return NULL;

    lista->prim = NULL;
    lista->ult = NULL;
    lista->largo = 0;

    return lista;
}

bool lista_esta_vacia(const lista_t *lista){
    return !lista->prim;
}

bool lista_insertar_primero(lista_t *lista, void *dato){
    nodo_t* nodo_nuevo = crear_nodo(dato);
    if (!nodo_nuevo) return false;

    if (!lista->ult) lista->ult = nodo_nuevo;

    nodo_nuevo->prox = lista->prim; //Funciona también para una lista vacía
    lista->prim = nodo_nuevo;

    lista->largo++;

    return true;
}

bool lista_insertar_ultimo(lista_t *lista, void *dato){
    nodo_t* nodo_nuevo = crear_nodo(dato);
    if (!nodo_nuevo) return false;

    if (!lista->prim) lista->prim = nodo_nuevo;

    else lista->ult->prox = nodo_nuevo;
    
    lista->ult = nodo_nuevo;

    lista->largo++;

    return true;

}

void *lista_borrar_primero(lista_t *lista){
    if (!lista->prim) return NULL;
    nodo_t* nodo_borrado = lista->prim;
    void* dato = lista->prim->dato;
    lista->prim = lista->prim->prox;
    if (!lista->prim) lista->ult = NULL;

    free(nodo_borrado);

    lista->largo--;

    return dato;
}

void *lista_ver_primero(const lista_t *lista){
    if (!lista->prim) return NULL;
    return lista->prim->dato;
}

void *lista_ver_ultimo(const lista_t* lista){
    if (!lista->prim) return NULL;
    return lista->ult->dato;
}

size_t lista_largo(const lista_t *lista){
    return lista->largo;
}

void lista_destruir(lista_t *lista, void(*destruir_dato)(void *)){
    while (!lista_esta_vacia(lista)){
        void* dato = lista_borrar_primero(lista);
        if (destruir_dato != NULL) destruir_dato(dato);
    }
    free(lista);
}

void lista_iterar(lista_t *lista, bool (*visitar)(void *dato, void *extra), void *extra){
    nodo_t* act = lista->prim;
    while (act){
        if (!visitar(act->dato, extra)) break;
        act = act->prox;
    }
}

lista_iter_t *lista_iter_crear(lista_t *lista){
    lista_iter_t* iter = malloc(sizeof(lista_iter_t));
    if (!iter) return NULL;

    iter->lista = lista;
    iter->act = lista->prim;
    iter->ant = NULL;

    return iter;
}

bool lista_iter_avanzar(lista_iter_t *iter){
    if (!iter->act) return false;
    iter->ant = iter->act;
    iter->act = iter->act->prox;
    return true;
}

void *lista_iter_ver_actual(const lista_iter_t *iter){
    if (!iter->act) return NULL;
    return iter->act->dato;
}

bool lista_iter_al_final(const lista_iter_t *iter){
    return !iter->act;
}

void lista_iter_destruir(lista_iter_t *iter){
    free(iter);
}

bool lista_iter_insertar(lista_iter_t *iter, void *dato){
    nodo_t* nodo_nuevo = crear_nodo(dato);
    if (!nodo_nuevo) return false;

    if (!iter->ant) iter->lista->prim = nodo_nuevo;
    else iter->ant->prox = nodo_nuevo;
    nodo_nuevo->prox = iter->act;
    iter->act = nodo_nuevo;
    if (!nodo_nuevo->prox) iter->lista->ult = nodo_nuevo;

    iter->lista->largo++;

    return true;
}

void *lista_iter_borrar(lista_iter_t *iter){
    if (!iter->act) return NULL;

    nodo_t* nodo_borrado = iter->act;
    void* dato = iter->act->dato;
    
    if (!iter->ant) iter->lista->prim = iter->act->prox;
    else iter->ant->prox = iter->act->prox;    

    iter->act = iter->act->prox;
    if(!iter->act) iter->lista->ult = iter->ant;

    free(nodo_borrado);

    iter->lista->largo--;
    
    return dato;
}