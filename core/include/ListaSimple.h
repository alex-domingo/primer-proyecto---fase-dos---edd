#ifndef PROYECTO_EDD_LISTASIMPLE_H
#define PROYECTO_EDD_LISTASIMPLE_H

#include "NodoLista.h"
#include <string>

/*
 * ListaSimple.h
 * -------------
 * Lista enlazada NO ordenada. Inserta siempre al frente
 * porque es la operación más rápida — O(1).
 *
 * Su búsqueda es secuencial O(n), lo que la hace lenta
 * comparada con el AVL o la tabla hash, pero eso es
 * exactamente lo que queremos demostrar en las pruebas
 * de rendimiento.
 *
 * Operaciones y su complejidad:
 *   insertar   -> O(1)  — siempre al frente
 *   buscar     -> O(n)  — recorre nodo por nodo
 *   eliminar   -> O(n)  — busca primero, luego desenlaza
 *   listar     -> O(n)  — recorre toda la lista
 */

class ListaSimple {
private:
    NodoLista *cabeza; // primer nodo de la lista
    int tamano; // cantidad de elementos

public:
    ListaSimple();

    ~ListaSimple();

    // Inserta al frente — O(1)
    void insertar(const Producto &producto);

    // Busca por nombre, retorna puntero al dato o nullptr si no existe — O(n)
    Producto *buscarPorNombre(const std::string &nombre);

    // Busca por código de barra — O(n)
    Producto *buscarPorCodigo(const std::string &codigo);

    // Elimina el nodo con ese código de barra — O(n)
    bool eliminar(const std::string &codigoBarra);

    // Imprime todos los productos en consola — O(n)
    void listar() const;

    // Retorna cuántos elementos hay en la lista
    int obtenerTamano() const;

    // Dice si la lista está vacía
    bool estaVacia() const;
};

#endif //PROYECTO_EDD_LISTASIMPLE_H
