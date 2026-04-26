#include "ListaOrdenada.h"
#include <iostream>

/*
 * ListaOrdenada.cpp
 * -----------------
 * La diferencia clave con ListaSimple es que al insertar
 * buscamos la posición correcta para que la lista siempre
 * quede ordenada por nombre.
 *
 * El truco del "corte anticipado" en la búsqueda: si el nombre
 * del nodo actual es mayor al que buscamos, ya sabemos que no
 * va a aparecer más adelante (porque está ordenado), así que
 * paramos ahí en vez de seguir hasta el final.
 */

ListaOrdenada::ListaOrdenada() : cabeza(nullptr), tamano(0) {
}

ListaOrdenada::~ListaOrdenada() {
    NodoLista *actual = cabeza;
    while (actual != nullptr) {
        NodoLista *siguiente = actual->siguiente;
        delete actual;
        actual = siguiente;
    }
}

// Recorre la lista hasta encontrar la posición donde debe ir el nuevo nodo
// para que la lista siga ordenada alfabéticamente
void ListaOrdenada::insertar(const Producto &producto) {
    NodoLista *nuevo = new NodoLista(producto);

    // Caso 1: lista vacía o el nuevo va antes que la cabeza actual
    if (cabeza == nullptr || producto.nombre <= cabeza->dato.nombre) {
        nuevo->siguiente = cabeza;
        cabeza = nuevo;
        tamano++;
        return;
    }

    // Caso 2: buscamos el punto de inserción recorriendo la lista
    NodoLista *actual = cabeza;
    while (actual->siguiente != nullptr &&
           actual->siguiente->dato.nombre < producto.nombre) {
        actual = actual->siguiente;
    }

    // Insertamos entre 'actual' y 'actual->siguiente'
    nuevo->siguiente = actual->siguiente;
    actual->siguiente = nuevo;
    tamano++;
}

// Búsqueda con corte anticipado: si el nodo actual tiene nombre > al buscado,
// ya no tiene sentido seguir buscando hacia adelante
Producto *ListaOrdenada::buscarPorNombre(const std::string &nombre) {
    NodoLista *actual = cabeza;
    while (actual != nullptr) {
        if (actual->dato.nombre == nombre) {
            return &actual->dato; // encontrado
        }
        // Corte anticipado: pasamos el punto donde debería estar
        if (actual->dato.nombre > nombre) {
            break;
        }
        actual = actual->siguiente;
    }
    return nullptr;
}

// Por código de barra no podemos usar corte anticipado
// porque la lista está ordenada por nombre, no por código
Producto *ListaOrdenada::buscarPorCodigo(const std::string &codigo) {
    NodoLista *actual = cabeza;
    while (actual != nullptr) {
        if (actual->dato.codigoBarra == codigo) {
            return &actual->dato;
        }
        actual = actual->siguiente;
    }
    return nullptr;
}

// La eliminación es similar a la de ListaSimple
bool ListaOrdenada::eliminar(const std::string &codigoBarra) {
    if (cabeza == nullptr) {
        return false;
    }

    // Caso especial: es la cabeza
    if (cabeza->dato.codigoBarra == codigoBarra) {
        NodoLista *aBorrar = cabeza;
        cabeza = cabeza->siguiente;
        delete aBorrar;
        tamano--;
        return true;
    }

    // Caso general: buscamos el anterior
    NodoLista *anterior = cabeza;
    while (anterior->siguiente != nullptr) {
        if (anterior->siguiente->dato.codigoBarra == codigoBarra) {
            NodoLista *aBorrar = anterior->siguiente;
            anterior->siguiente = aBorrar->siguiente;
            delete aBorrar;
            tamano--;
            return true;
        }
        anterior = anterior->siguiente;
    }

    return false;
}

// Como la lista ya está ordenada, el listado sale alfabético automáticamente
void ListaOrdenada::listar() const {
    if (cabeza == nullptr) {
        std::cout << "[Lista ordenada] La lista esta vacia.\n";
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

int ListaOrdenada::obtenerTamano() const {
    return tamano;
}

bool ListaOrdenada::estaVacia() const {
    return cabeza == nullptr;
}
