#ifndef PROYECTO_EDD_NODOLISTA_H
#define PROYECTO_EDD_NODOLISTA_H

#include "Producto.h"

/*
 * NodoLista.h
 * -----------
 * Un nodo es la unidad básica de cualquier lista enlazada.
 * Guarda un producto y un puntero al siguiente nodo.
 *
 * Lo usamos tanto en la lista simple como en la ordenada,
 * así evitamos duplicar código.
 */

struct NodoLista {
    Producto dato; // el producto que almacena este nodo
    NodoLista *siguiente; // apunta al siguiente nodo (nullptr si es el último)

    // Constructor — recibe el producto y deja siguiente en nullptr
    explicit NodoLista(const Producto &producto);
};

#endif //PROYECTO_EDD_NODOLISTA_H
