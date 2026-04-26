#ifndef PROYECTO_EDD_LISTAORDENADA_H
#define PROYECTO_EDD_LISTAORDENADA_H

#include "NodoLista.h"
#include <string>

/*
 * ListaOrdenada.h
 * ---------------
 * Lista enlazada que mantiene los productos ordenados
 * alfabéticamente por nombre en todo momento.
 *
 * La ventaja sobre la lista simple es que al buscar podemos
 * parar antes si ya pasamos el punto donde debería estar
 * el elemento (corte anticipado). Aun así sigue siendo O(n).
 *
 * Operaciones y su complejidad:
 *   insertar   -> O(n)  — busca la posición correcta antes de insertar
 *   buscar     -> O(n)  — pero con corte anticipado cuando nombre > buscado
 *   eliminar   -> O(n)  — busca y desenlaza
 *   listar     -> O(n)  — ya sale en orden alfabético
 */

class ListaOrdenada {
private:
    NodoLista *cabeza;
    int tamano;

public:
    ListaOrdenada();

    ~ListaOrdenada();

    // Inserta manteniendo el orden alfabético por nombre — O(n)
    void insertar(const Producto &producto);

    // Busca por nombre con corte anticipado — O(n)
    Producto *buscarPorNombre(const std::string &nombre);

    // Busca por código de barra — O(n) sin corte anticipado
    Producto *buscarPorCodigo(const std::string &codigo);

    // Elimina el nodo con ese código de barra — O(n)
    bool eliminar(const std::string &codigoBarra);

    // Lista en orden alfabético (que es el orden natural de la lista)
    void listar() const;

    int obtenerTamano() const;

    bool estaVacia() const;
};

#endif //PROYECTO_EDD_LISTAORDENADA_H
