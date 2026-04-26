#include "NodoBPlus.h"

/*
 * NodoBPlus.cpp
 * -------------
 * Todo nodo nace como hoja, sin claves y sin enlaces.
 * El puntero 'siguiente' es lo que hace especial al B+:
 * conecta todas las hojas en una lista de izquierda a derecha.
 */

NodoBPlus::NodoBPlus() : siguiente(nullptr), numClaves(0), esHoja(true) {
    for (int i = 0; i < MAX_HIJOS_BP; i++) {
        hijos[i] = nullptr;
    }
}
