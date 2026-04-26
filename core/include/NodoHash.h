#ifndef NODO_HASH_H
#define NODO_HASH_H

#include "Producto.h"

/*
 * NodoHash.h
 * ----------
 * Cada "bucket" de la tabla hash es en realidad una lista
 * enlazada de nodos. Cuando dos códigos de barra caen en
 * la misma posición (colisión), simplemente encadenamos
 * los nodos en esa posición — eso es el "chaining".
 *
 * Elegimos encadenamiento separado porque:
 *   - La eliminación es más limpia que en open addressing
 *   - El factor de carga puede superar 1 sin problema grave
 *   - Es más fácil de entender y depurar
 */

struct NodoHash {
    Producto  dato;
    NodoHash *siguiente; // siguiente nodo en el mismo bucket

    explicit NodoHash(const Producto &producto);
};

#endif // NODO_HASH_H
