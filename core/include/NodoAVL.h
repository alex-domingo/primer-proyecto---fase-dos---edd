#ifndef PROYECTO_EDD_NODOAVL_H
#define PROYECTO_EDD_NODOAVL_H

#include "Producto.h"

/*
 * NodoAVL.h
 * ----------
 * Cada nodo del árbol AVL guarda un producto y además
 * necesita saber su altura para poder calcular el balance
 * y decidir cuándo rotar.
 *
 * A diferencia del nodo de lista, aquí tenemos dos punteros:
 * izquierda (productos con nombre menor) y
 * derecha   (productos con nombre mayor).
 */

struct NodoAVL {
    Producto dato;
    NodoAVL *izquierda;
    NodoAVL *derecha;
    int altura; // altura del subárbol que tiene este nodo como raíz

    explicit NodoAVL(const Producto &producto);
};

#endif //PROYECTO_EDD_NODOAVL_H
