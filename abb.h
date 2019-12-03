#ifndef ABB_H
#define ABB_H

#include <stdbool.h>
#include <stddef.h>

// El struct debe llamarse "abb".
typedef struct abb abb_t;

// tipo de función para comparar claves y destruir dato
typedef int (*abb_comparar_clave_t) (const char *, const char *);
typedef void (*abb_destruir_dato_t) (void *);

/* Crea el abb
 */
abb_t* abb_crear(abb_comparar_clave_t cmp, abb_destruir_dato_t destruir_dato);

/* Guarda un elemento en el abb. De no poder guardarlo 
 * devuelve false.
 * Pre: La estructura abb fue inicializada
 * Post: Se almacenó el par (clave, dato)
 */
bool abb_guardar(abb_t *arbol, const char *clave, void *dato);

/* Borra un elemento del abb y devuelve el dato asociado.  Devuelve
 * NULL si el dato no estaba.
 * Pre: La estructura abb fue inicializada
 * Post: El elemento fue borrado de la estructura y se lo devolvió,
 * en el caso de que estuviera guardado.
 */
void *abb_borrar(abb_t *arbol, const char *clave);

/* Obtiene el valor de un elemento del abb, si la clave no se encuentra
 * devuelve NULL.
 * Pre: La estructura hash fue inicializada
 */
void *abb_obtener(const abb_t *arbol, const char *clave);

/* Determina si clave pertenece o no al abb.
 * Pre: La estructura hash fue inicializada
 */
bool abb_pertenece(const abb_t *arbol, const char *clave);

/* Devuelve la cantidad de elementos del abb.
 * Pre: La estructura hash fue inicializada
 */
size_t abb_cantidad(abb_t *arbol);

/* Destruye la estructura liberando la memoria pedida y llamando a la función
 * destruir para cada par (clave, dato).
 * Pre: La estructura abb fue inicializada
 * Post: La estructura abb fue destruida
 */
void abb_destruir(abb_t *arbol);

// Itera el abb de forma in order y aplica una funcion que recibe por parametro
// Pre: el abb fue creado
// Post: se aplico la funcion visitar que puede o no haber modificado el abb
void abb_in_order(abb_t *arbol, bool visitar(const char *, void *, void *), void *extra);


/* Iterador del abb */

// Define como debe llamarse el struct
typedef struct abb_iter abb_iter_t;

// Crea iterador
// Pre: el abb fue creada
// Post: devuelve un nuevo iterador apuntando al 
// primer nodo de el abb
abb_iter_t *abb_iter_in_crear(const abb_t *arbol);

// Avanza iterador
// Pre: el iterador fue creado y el iterador apunta a un nodo
// Post: el iterador avanza al siguiente nodo y devuelve true, 
//  si no puede devuelve false
bool abb_iter_in_avanzar(abb_iter_t *iter);

// Muestra el dato del nodo donde esta posicionado el iterador
// Pre: el iterador fue creado y apunta a un nodo
// Post: devuelve el dato del nodo o NULL de no ser posible
const char *abb_iter_in_ver_actual(const abb_iter_t *iter);

// Devuelve true si el iterador esta al final, false en caso contrario
// Pre: el iterador fue creado y apunta a un nodo
bool abb_iter_in_al_final(const abb_iter_t *iter);

// Destruye el iterador de el abb
// Pre: el iterador fue creado
// Post: el iterador fue destruido
void abb_iter_in_destruir(abb_iter_t* iter);

#endif // ABB_H
