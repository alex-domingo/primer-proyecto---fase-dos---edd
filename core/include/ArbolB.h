#ifndef PROYECTO_EDD_ARBOLB_H
#define PROYECTO_EDD_ARBOLB_H

#include "NodoB.h"
#include <string>

/*
 * ArbolB.h
 * ---------
 * Árbol B de orden t=3, indexado por fecha de caducidad (YYYY-MM-DD).
 *
 * La gran ventaja del árbol B sobre el AVL para este caso es que
 * soporta búsquedas por RANGO de fechas eficientemente, por ejemplo:
 * "dame todos los productos que caducan entre 2026-01-01 y 2026-06-30".
 *
 * Complejidades:
 *   Insertar  -> O(log n)
 *   Buscar    -> O(log n)
 *   Eliminar  -> O(log n)
 *   Rango     -> O(log n + k)  donde k es la cantidad de resultados
 *
 * Métodos privados clave:
 *   dividirHijo   — cuando un nodo está lleno, lo partimos en dos
 *   insertarNoLleno — inserta en un nodo que sabemos que no está lleno
 *   buscarEnNodo  — búsqueda dentro de un nodo específico
 *   eliminarRec   — eliminación recursiva con merge/redistribución
 */

class ArbolB {
private:
    NodoB *raiz;
    int tamano;

    // Divide el hijo hijos[indice] del nodo padre cuando está lleno
    void dividirHijo(NodoB *padre, int indice);

    // Inserta en un nodo que garantizamos que no está lleno
    void insertarNoLleno(NodoB *nodo, const std::string &clave,
                         const Producto &producto);

    // Búsqueda recursiva — retorna el nodo y el índice donde está la clave
    Producto *buscarEnNodo(NodoB *nodo, const std::string &clave) const;

    // Recolecta todos los productos en el rango [inicio, fin]
    void buscarRangoRec(NodoB *nodo, const std::string &inicio,
                        const std::string &fin) const;

    // -- Auxiliares para la eliminación --
    void eliminarRec(NodoB *nodo, const std::string &clave);

    void eliminarDeHoja(NodoB *nodo, int idx);

    void eliminarDeInterior(NodoB *nodo, int idx);

    void rellenar(NodoB *nodo, int idx);

    void tomarDePrevio(NodoB *nodo, int idx);

    void tomarDeSiguiente(NodoB *nodo, int idx);

    void fusionar(NodoB *nodo, int idx);

    // Encuentra el predecesor in-order (la clave más grande del subárbol izq)
    std::string obtenerPredecesor(NodoB *nodo, int idx);

    // Encuentra el sucesor in-order (la clave más pequeña del subárbol der)
    std::string obtenerSucesor(NodoB *nodo, int idx);

    // Libera toda la memoria del árbol
    void destruirArbol(NodoB *nodo);

public:
    ArbolB();

    ~ArbolB();

    // Inserta un producto usando su fecha como clave — O(log n)
    void insertar(const Producto &producto);

    // Busca un producto por fecha exacta — O(log n)
    Producto *buscar(const std::string &fecha) const;

    // Lista todos los productos cuya fecha esté en [inicio, fin] — O(log n + k)
    void buscarRango(const std::string &fechaInicio,
                     const std::string &fechaFin) const;

    // Elimina el producto con esa fecha de caducidad — O(log n)
    bool eliminar(const std::string &fecha);

    // Lista todos los productos en orden de fecha (in-order)
    void listarEnOrden() const;

    int obtenerTamano() const;

    bool estaVacio() const;

    // -- Acceso interno para VisualizadorDot --
    NodoB *obtenerRaiz() const { return raiz; }

private:
    // Auxiliar para listar en orden (recorre recursivamente)
    void listarRec(NodoB *nodo) const;
};

#endif //PROYECTO_EDD_ARBOLB_H
