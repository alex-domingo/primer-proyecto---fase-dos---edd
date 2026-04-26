#include "NodoAVL.h"

/*
 * NodoAVL.cpp
 * -----------
 * Un nodo nuevo siempre empieza como hoja:
 * sin hijos y con altura 1 (él mismo).
 */

NodoAVL::NodoAVL(const Producto &producto)
    : dato(producto), izquierda(nullptr), derecha(nullptr), altura(1) {
}
