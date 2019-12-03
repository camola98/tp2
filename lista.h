#include <stdlib.h>
#include <stdbool.h>


/* ******************************************************************
 *                DEFINICION DE LOS TIPOS DE DATOS
 * *****************************************************************/

/* La lista está planteada como una lista de punteros genéricos. */

typedef struct lista lista_t;
typedef struct lista_iter lista_iter_t;


/* ******************************************************************
 *                    PRIMITIVAS DE LA LISTA
 * *****************************************************************/

// Crea una lista.
// Post: devuelve una nueva lista vacía.
lista_t* lista_crear(void);

// Destruye la lista. Si se recibe la función destruir_dato por parámetro,
// para cada uno de los elementos de la lista llama a destruir_dato.
// Pre: la lista fue creada. destruir_dato es una función capaz de destruir
// los datos de la lista, o NULL en caso de que no se la utilice.
// Post: se eliminaron todos los elementos de la lista.
void lista_destruir(lista_t *lista, void destruir_dato(void *));

// Devuelve verdadero si la lista no tiene elementos, false en caso contrario.
// Pre: la lista fue creada.
bool lista_esta_vacia(const lista_t *lista);

// Inserta un nuevo elemento al principio de la lista. Devuelve falso en caso de error.
// Pre: la lista fue creada.
// Post: se insertó un nuevo elemento al principio de la lista.
bool lista_insertar_primero(lista_t *lista, void *dato);

// Inserta un nuevo elemento al final de la lista. Devuelve falso en caso de error.
// Pre: la lista fue creada.
// Post: se insertó un nuevo elemento al final de la lista.
bool lista_insertar_ultimo(lista_t *lista, void *dato);

// Obtiene el valor del primer elemento de la lista. Si la lista tiene
// elementos, se devuelve el valor del primero, si está vacía devuelve NULL.
// Pre: la lista fue creada.
// Post: se devolvió el primer elemento de la lista, cuando no está vacía.
void *lista_ver_primero(const lista_t *lista);

// Obtiene el valor del último elemento de la lista. Si la lista tiene
// elementos, se devuelve el valor del último, si está vacía devuelve NULL.
// Pre: la lista fue creada.
// Post: se devolvió el último elemento de la lista, cuando no está vacía.
void *lista_ver_ultimo(const lista_t* lista);

// Borra el primer elemento de la lista. Si la lista tiene elementos, se quita el
// primero de la cola, y se devuelve su valor, si está vacía, devuelve NULL.
// Pre: la lista fue creada.
// Post: se devolvió el valor del primer elemento anterior, la lista
// contiene un elemento menos, si la lista no estaba vacía.
void *lista_borrar_primero(lista_t *lista);

// Devuelve el tamaño de la lista.
// Pre: la lista fue creada.
size_t lista_largo(const lista_t *lista);

/* ******************************************************************
 *                PRIMITIVAS DEL ITERADOR DE LA LISTA
 * *****************************************************************/
// Crea un iterador de lista.
// Pre: la lista fue creada.
// Post: devuelve un iterador de lista que comienza en el elemento 0.
lista_iter_t *lista_iter_crear(lista_t *lista);

// Avanza el iterador de lista en una posición. Devuelve falso en caso de que
// ya estuviera en la última.
// Pre: el iterador fue creado.
// Post: se avanzó el iterador de la lista en una posición, si no se 
// encontraba en la última.
bool lista_iter_avanzar(lista_iter_t *iter);

// Obtiene el valor del elemento en la posición sobre la que se encuentra el 
// iterador. Si está sobre la última posición, devuelve NULL.
// Pre: el iterador fue creado.
// Post: se devolvió el valor el elemento en la posición que se encuentra el
// iterador, si esta no era la última.
void *lista_iter_ver_actual(const lista_iter_t *iter);

// Devuelve true si el iterador se encuentra en la última posición.
// False en caso contrario.
// Pre: el iterador fue creado. 
bool lista_iter_al_final(const lista_iter_t *iter);

// Destruye el iterador de lista.
// Pre: el iterador fue creado.
void lista_iter_destruir(lista_iter_t *iter);

// Inserta un nuevo elemento en la posición que se encuentra el iterador. 
// Devuelve falso en caso de error.
// Pre: el iterador fue creado.
// Post: se insertó un nuevo elemento en la posición actual del iterador.
bool lista_iter_insertar(lista_iter_t *iter, void *dato);

// Borra el elemento en la posición que se encuentra el iterador y devuelve su 
// valor. Si estaba en la última posición, devuelve NULL.
// Pre: el iterador fue creado.
// Post: se borró el elemento en la posición actual del iterador, si esta no
//  era la última.
void *lista_iter_borrar(lista_iter_t *iter);

// Aplica la función "visitar" a todos los elementos de la lista. Opcionalmente,
// puede recibir una condición de corte mediante el parámetro extra.
// Pre: la lista fue creada.
// Post: se le aplicó la función "visitar" a todos los elementos de la lista.
void lista_iterar(lista_t *lista, bool (*visitar)(void *dato, void *extra), void *extra);

/* *****************************************************************
 *                      PRUEBAS UNITARIAS
 * *****************************************************************/

// Realiza pruebas sobre la implementación del alumno.
//
// Las pruebas deben emplazarse en el archivo ‘pruebas_alumno.c’, y
// solamente pueden emplear la interfaz pública tal y como aparece en cola.h
// (esto es, las pruebas no pueden acceder a los miembros del struct cola).
//
// Para la implementación de las pruebas se debe emplear la función
// print_test(), como se ha visto en TPs anteriores.
void pruebas_lista_alumno(void);