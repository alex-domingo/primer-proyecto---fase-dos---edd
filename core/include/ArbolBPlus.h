#ifndef PROYECTO_EDD_ARBOLBPLUS_H
#define PROYECTO_EDD_ARBOLBPLUS_H

#include "NodoBPlus.h"
#include <string>

/*
 * ArbolBPlus.h
 * -------------
 * Árbol B+ de orden t=3, indexado por categoría del producto.
 *
 * Diferencias clave con el árbol B:
 *   1. Los datos solo viven en las HOJAS
 *   2. Las hojas están enlazadas entre sí (lista doble implícita)
 *   3. Los nodos internos solo guardan claves para navegar
 *   4. Una misma categoría puede aparecer en MÚLTIPLES hojas
 *      (varios productos de la misma categoría)
 *
 * La operación estrella es buscarPorCategoria:
 *   - Bajamos O(log n) hasta la primera hoja con esa categoría
 *   - Luego caminamos la cadena de hojas hacia la derecha O(k)
 *   - Total: O(log n + k) donde k es la cantidad de resultados
 *
 * Complejidades:
 *   Insertar          -> O(log n)
 *   Buscar exacto     -> O(log n)
 *   Buscar categoría  -> O(log n + k)
 *   Eliminar          -> O(log n)
 */

class ArbolBPlus {
private:
    NodoBPlus *raiz;
    NodoBPlus *primeraHoja; // puntero directo a la hoja más a la izquierda
    int tamano;

    // Divide el hijo en el índice dado cuando está lleno
    void dividirHijo(NodoBPlus *padre, int indice, NodoBPlus *hijo);

    // Inserta en un nodo que no está lleno
    void insertarNoLleno(NodoBPlus *nodo, const std::string &clave,
                         const Producto &producto);

    // Búsqueda recursiva en el árbol
    Producto *buscarEnNodo(NodoBPlus *nodo, const std::string &clave) const;

    // Auxiliares de eliminación
    void eliminarRec(NodoBPlus *nodo, const std::string &clave,
                     const std::string &codigoBarra);

    void eliminarDeHoja(NodoBPlus *nodo, int idx);

    void rellenarBPlus(NodoBPlus *padre, int idx);

    void tomarDePrevioBP(NodoBPlus *padre, int idx);

    void tomarDeSiguienteBP(NodoBPlus *padre, int idx);

    void fusionarBPlus(NodoBPlus *padre, int idx);

    // Libera toda la memoria
    void destruirArbol(NodoBPlus *nodo);

public:
    ArbolBPlus();

    ~ArbolBPlus();

    // Inserta un producto usando su categoría como clave — O(log n)
    void insertar(const Producto &producto);

    // Busca un producto por categoría y nombre exacto — O(log n)
    Producto *buscar(const std::string &categoria,
                     const std::string &nombre) const;

    // Lista todos los productos de una categoría recorriendo las hojas — O(log n + k)
    void buscarPorCategoria(const std::string &categoria) const;

    // Elimina un producto específico por categoría y código de barra — O(log n)
    bool eliminar(const std::string &categoria,
                  const std::string &codigoBarra);

    // Lista todos los productos en orden de categoría
    void listarEnOrden() const;

    int obtenerTamano() const;

    bool estaVacio() const;

    // -- Acceso interno para VisualizadorDot --
    NodoBPlus *obtenerRaiz() const { return raiz; }
    NodoBPlus *obtenerPrimeraHoja() const { return primeraHoja; }
};

#endif //PROYECTO_EDD_ARBOLBPLUS_H
