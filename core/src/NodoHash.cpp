#include "NodoHash.h"

/*
 * NodoHash.cpp
 * ------------
 * Igual que el nodo de lista: guarda el producto
 * y deja el puntero al siguiente en nullptr.
 */

NodoHash::NodoHash(const Producto &producto)
    : dato(producto), siguiente(nullptr) {
}
