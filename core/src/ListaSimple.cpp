#include "ListaSimple.h"
#include <iostream>

/*
 * ListaSimple.cpp
 * ---------------
 * Implementación de la lista enlazada no ordenada.
 *
 * La idea es simple: cada nodo apunta al siguiente,
 * el último apunta a nullptr, y nosotros siempre
 * guardamos un puntero al primero (cabeza).
 */

ListaSimple::ListaSimple() : cabeza(nullptr), tamano(0) {
}

// El destructor recorre y libera cada nodo de memoria
ListaSimple::~ListaSimple() {
    NodoLista *actual = cabeza;
    while (actual != nullptr) {
        NodoLista *siguiente = actual->siguiente;
        delete actual;
        actual = siguiente;
    }
}

// Insertar al frente es O(1): el nuevo nodo apunta a la cabeza actual
// y luego se convierte en la nueva cabeza
void ListaSimple::insertar(const Producto &producto) {
    NodoLista *nuevo = new NodoLista(producto);
    nuevo->siguiente = cabeza;
    cabeza = nuevo;
    tamano++;
}

// Búsqueda secuencial por nombre — revisamos nodo por nodo hasta encontrarlo
Producto *ListaSimple::buscarPorNombre(const std::string &nombre) {
    NodoLista *actual = cabeza;
    while (actual != nullptr) {
        if (actual->dato.nombre == nombre) {
            return &actual->dato; // encontrado, retornamos su dirección
        }
        actual = actual->siguiente;
    }
    return nullptr; // no estaba en la lista
}

// Igual que buscarPorNombre pero comparamos el código de barra
Producto *ListaSimple::buscarPorCodigo(const std::string &codigo) {
    NodoLista *actual = cabeza;
    while (actual != nullptr) {
        if (actual->dato.codigoBarra == codigo) {
            return &actual->dato;
        }
        actual = actual->siguiente;
    }
    return nullptr;
}

// Para eliminar necesitamos encontrar el nodo anterior al que queremos borrar
// porque tenemos que "saltarnos" ese nodo en la cadena de punteros
bool ListaSimple::eliminar(const std::string &codigoBarra) {
    if (cabeza == nullptr) {
        return false; // lista vacía, nada que eliminar
    }

    // Caso especial: el nodo a eliminar es la cabeza
    if (cabeza->dato.codigoBarra == codigoBarra) {
        NodoLista *aBorrar = cabeza;
        cabeza = cabeza->siguiente;
        delete aBorrar;
        tamano--;
        return true;
    }

    // Caso general: buscamos el nodo anterior al que queremos borrar
    NodoLista *anterior = cabeza;
    while (anterior->siguiente != nullptr) {
        if (anterior->siguiente->dato.codigoBarra == codigoBarra) {
            NodoLista *aBorrar = anterior->siguiente;
            anterior->siguiente = aBorrar->siguiente; // nos saltamos el nodo
            delete aBorrar;
            tamano--;
            return true;
        }
        anterior = anterior->siguiente;
    }

    return false; // no se encontró el código
}

// Recorre e imprime todos los productos
void ListaSimple::listar() const {
    if (cabeza == nullptr) {
        std::cout << "[Lista simple] La lista esta vacia.\n";
        return;
    }

    NodoLista *actual = cabeza;
    int indice = 1;
    while (actual != nullptr) {
        std::cout << indice++ << ". ";
        actual->dato.mostrar();
        actual = actual->siguiente;
    }
}

int ListaSimple::obtenerTamano() const {
    return tamano;
}

bool ListaSimple::estaVacia() const {
    return cabeza == nullptr;
}
