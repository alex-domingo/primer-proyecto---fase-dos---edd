#ifndef CATALOGO_H
#define CATALOGO_H

#include "Producto.h"
#include "ListaSimple.h"
#include "ListaOrdenada.h"
#include "ArbolAVL.h"
#include "ArbolB.h"
#include "ArbolBPlus.h"
#include "TablaHash.h"

/*
 * Catalogo.h — Fase 2
 * --------------------
 * Orquestador de las 6 estructuras de datos.
 * La TablaHash se integra como sexta estructura indexada
 * por código de barra (búsqueda O(1) amortizado).
 *
 * Orden de inserción atómica:
 *   1. listaSimple
 *   2. listaOrdenada
 *   3. arbolAVL      ← falla si nombre duplicado → rollback 1-2
 *   4. tablaHash     ← falla si código duplicado → rollback 1-3
 *   5. arbolB
 *   6. arbolBPlus
 */

class Catalogo {
private:
    ListaSimple   *listaSimple;
    ListaOrdenada *listaOrdenada;
    ArbolAVL      *arbolAVL;
    ArbolB        *arbolB;
    ArbolBPlus    *arbolBPlus;
    TablaHash     *tablaHash;    // ← NUEVA — Fase 2

    int totalProductos;

    bool existeCodigo(const std::string &codigoBarra) const;

public:
    Catalogo();
    ~Catalogo();

    // Inserción atómica en las 6 estructuras con rollback
    bool agregarProducto(const Producto &producto);

    // Eliminación propagada a las 6 estructuras
    bool eliminarProducto(const std::string &nombre,
                          const std::string &codigoBarra,
                          const std::string &categoria,
                          const std::string &fechaCaducidad);

    // ── Búsquedas ────────────────────────────────────────────
    Producto *buscarPorNombre(const std::string &nombre);
    Producto *buscarPorCodigo(const std::string &codigoBarra); // ← NUEVA vía Hash
    void      buscarPorCategoria(const std::string &categoria);
    void      buscarPorRangoFecha(const std::string &inicio,
                             const std::string &fin);

    // ── Listados ─────────────────────────────────────────────
    void listarPorNombre()  const;
    void listarSimple()     const;
    void listarOrdenado()   const;

    int  contarProductos()  const;
    bool estaVacio()        const;

    // Actualiza el stock de un producto en todas las estructuras — O(n) listas + O(log n) AVL
    // Usar cuando el stock cambia sin eliminar/reinsertar el producto
    bool actualizarStock(const std::string &codigoBarra, int nuevoStock);

    // ── Acceso a estructuras (benchmark + visualizador) ──────
    ListaSimple   *obtenerListaSimple()   const { return listaSimple;   }
    ListaOrdenada *obtenerListaOrdenada() const { return listaOrdenada; }
    ArbolAVL      *obtenerArbolAVL()      const { return arbolAVL;      }
    ArbolB        *obtenerArbolB()        const { return arbolB;        }
    ArbolBPlus    *obtenerArbolBPlus()    const { return arbolBPlus;    }
    TablaHash     *obtenerTablaHash()     const { return tablaHash;     }
};

#endif // CATALOGO_H
