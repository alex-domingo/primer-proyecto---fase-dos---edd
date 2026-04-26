#include "NodoB.h"

/*
 * NodoB.cpp
 * ----------
 * El constructor inicializa todos los punteros a hijos en nullptr
 * y el contador de claves en 0. Todo nodo nace como hoja.
 */

NodoB::NodoB() : numClaves(0), esHoja(true) {
    // Inicializamos todos los hijos en nullptr
    for (int i = 0; i < MAX_HIJOS; i++) {
        hijos[i] = nullptr;
    }
}
