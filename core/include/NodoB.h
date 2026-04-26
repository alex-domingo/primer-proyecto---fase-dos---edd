#ifndef PROYECTO_EDD_NODOB_H
#define PROYECTO_EDD_NODOB_H

#include "Producto.h"

/*
 * NodoB.h
 * --------
 * Un nodo del árbol B es muy diferente, en lugar de tener una sola
 * clave y dos hijos, aquí cada nodo puede tener VARIAS claves y VARIOS hijos.
 *
 * Para un árbol B de orden t:
 *   - Cada nodo tiene MÍNIMO (t-1) claves  (excepto la raíz)
 *   - Cada nodo tiene MÁXIMO (2t-1) claves
 *   - Cada nodo interno tiene exactamente (numClaves + 1) hijos
 *
 * Nosotros usamos t=3, así que:
 *   - Mínimo 2 claves por nodo (excepto raíz)
 *   - Máximo 5 claves por nodo
 *   - Máximo 6 hijos por nodo
 *
 * La clave que usamos es la fecha de caducidad (YYYY-MM-DD).
 * La comparación funciona perfectamente para ordenarlas cronológicamente.
 */

// Orden del árbol B — con t=3 los nodos tienen entre 2 y 5 claves
static const int ORDEN_B = 3;
static const int MAX_CLAVES = 2 * ORDEN_B - 1; // 5
static const int MAX_HIJOS = 2 * ORDEN_B; // 6

struct NodoB {
    // Arreglos de claves (fechas) y productos asociados
    std::string claves[MAX_CLAVES]; // fechas de caducidad ordenadas
    Producto productos[MAX_CLAVES]; // producto asociado a cada clave

    // Punteros a los hijos (un nodo interno tiene numClaves+1 hijos)
    NodoB *hijos[MAX_HIJOS];

    int numClaves; // cuántas claves tiene actualmente este nodo
    bool esHoja; // true si no tiene hijos

    NodoB();
};

#endif //PROYECTO_EDD_NODOB_H
