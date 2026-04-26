#ifndef PROYECTO_EDD_ARBOLAVL_H
#define PROYECTO_EDD_ARBOLAVL_H

#include "NodoAVL.h"
#include <string>

/*
 * ArbolAVL.h
 * -----------
 * Árbol Binario de Búsqueda auto-balanceado (AVL).
 * Se ordena por nombre del producto.
 *
 * La propiedad clave del AVL es que la diferencia de altura
 * entre el subárbol izquierdo y el derecho de cualquier nodo
 * nunca puede ser mayor a 1. Cuando eso se rompe tras una
 * inserción o eliminación, hacemos rotaciones para restaurarlo.
 *
 * Eso nos garantiza que la altura siempre sea O(log n),
 * y por tanto buscar, insertar y eliminar también son O(log n).
 *
 * Métodos públicos:
 *   insertar, buscar, eliminar, listarEnOrden, obtenerTamano
 *
 * Métodos privados (auxiliares recursivos + rotaciones):
 *   insertarRec, eliminarRec, buscarRec,
 *   rotarDerecha, rotarIzquierda,
 *   obtenerAltura, obtenerBalance, actualizarAltura,
 *   obtenerMinimo, destruirArbol, listarRec
 */

class ArbolAVL {
private:
    NodoAVL *raiz;
    int tamano;

    // -- Auxiliares de altura y balance --
    int obtenerAltura(NodoAVL *nodo) const;

    int obtenerBalance(NodoAVL *nodo) const;

    void actualizarAltura(NodoAVL *nodo);

    // -- Rotaciones para rebalancear --
    NodoAVL *rotarDerecha(NodoAVL *y);

    NodoAVL *rotarIzquierda(NodoAVL *x);

    // -- Rebalanceo general (decide qué rotación aplicar) --
    NodoAVL *rebalancear(NodoAVL *nodo);

    // -- Operaciones recursivas --
    NodoAVL *insertarRec(NodoAVL *nodo, const Producto &producto);

    NodoAVL *eliminarRec(NodoAVL *nodo, const std::string &nombre);

    Producto *buscarRec(NodoAVL *nodo, const std::string &nombre) const;

    // Encuentra el nodo con el nombre más pequeño (para la eliminación)
    NodoAVL *obtenerMinimo(NodoAVL *nodo) const;

    // Recorrido in-order para listar en orden alfabético
    void listarRec(NodoAVL *nodo) const;

    // Libera toda la memoria del árbol
    void destruirArbol(NodoAVL *nodo);

public:
    ArbolAVL();

    ~ArbolAVL();

    // Inserta un producto — O(log n)
    void insertar(const Producto &producto);

    // Busca por nombre exacto — O(log n)
    Producto *buscar(const std::string &nombre) const;

    // Elimina por nombre — O(log n)
    bool eliminar(const std::string &nombre);

    // Lista todos los productos en orden alfabético — O(n)
    void listarEnOrden() const;

    int obtenerTamano() const;

    bool estaVacio() const;

    // -- Acceso interno para VisualizadorDot --
    NodoAVL *obtenerRaiz() const { return raiz; }
};

#endif //PROYECTO_EDD_ARBOLAVL_H
