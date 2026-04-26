#ifndef PROYECTO_EDD_NODOBPLUS_H
#define PROYECTO_EDD_NODOBPLUS_H

#include "Producto.h"

/*
 * NodoBPlus.h
 * ------------
 * El nodo del árbol B+ tiene dos "modos" según si es hoja o no:
 *
 * NODO INTERNO:
 *   - Solo guarda claves de navegación (categorías)
 *   - Sus hijos[] apuntan a otros nodos
 *   - NO guarda productos directamente
 *
 * NODO HOJA:
 *   - Guarda las claves Y los productos reales
 *   - hijos[] no se usa (siempre nullptr)
 *   - siguiente apunta a la hoja siguiente (lista enlazada de hojas)
 *
 * Esta distinción es la diferencia fundamental entre B y B+.
 * En el B+ todos los datos viven en las hojas, los nodos
 * internos solo sirven para navegar.
 *
 * Usamos el mismo orden t=3 que el árbol B:
 *   Mínimo 2 claves por nodo (excepto raíz)
 *   Máximo 5 claves por nodo
 */

static const int ORDEN_BPLUS = 3;
static const int MAX_CLAVES_BP = 2 * ORDEN_BPLUS - 1; // 5
static const int MAX_HIJOS_BP = 2 * ORDEN_BPLUS; // 6

struct NodoBPlus {
    std::string claves[MAX_CLAVES_BP]; // claves de categoría
    Producto productos[MAX_CLAVES_BP]; // solo se usa en hojas

    NodoBPlus *hijos[MAX_HIJOS_BP]; // solo se usa en nodos internos
    NodoBPlus *siguiente; // enlace al siguiente nodo hoja

    int numClaves;
    bool esHoja;

    NodoBPlus();
};

#endif //PROYECTO_EDD_NODOBPLUS_H
