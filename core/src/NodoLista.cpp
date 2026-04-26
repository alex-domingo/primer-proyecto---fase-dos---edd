#include "NodoLista.h"

/*
 * NodoLista.cpp
 * -------------
 * Solo tiene el constructor del nodo.
 * Inicializamos siguiente en nullptr porque un nodo
 * recién creado todavía no apunta a nadie.
 */

NodoLista::NodoLista(const Producto &producto)
    : dato(producto), siguiente(nullptr) {
}
