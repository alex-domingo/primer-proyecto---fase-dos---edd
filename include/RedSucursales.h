#ifndef RED_SUCURSALES_H
#define RED_SUCURSALES_H

#include "Sucursal.h"
#include <string>
#include <vector>

/*
 * RedSucursales.h
 * ----------------
 * Grafo ponderado de sucursales implementado con lista de adyacencia.
 * Cada nodo es una Sucursal y cada arista tiene dos pesos:
 *   - tiempo : segundos de traslado entre sucursales
 *   - costo  : costo monetario del traslado
 *
 * Soporta aristas bidireccionales y unidireccionales.
 *
 * Algoritmo de ruta óptima: Dijkstra.
 * El usuario elige el criterio: TIEMPO o COSTO.
 *
 * Complejidades:
 *   agregarSucursal  -> O(1)
 *   agregarConexion  -> O(1)
 *   dijkstra         -> O((V + E) log V)  con cola de prioridad
 *   obtenerRuta      -> O(V)
 */

struct Conexion {
    std::string origenId;
    std::string destinoId;
    double      tiempo;       // peso tiempo
    double      costo;        // peso costo
    bool        bidireccional;
};

struct ResultadoRuta {
    std::vector<std::string> nodos;   // IDs de sucursales en orden
    double                   pesoTotal;
    bool                     encontrada;
};

class RedSucursales {
public:
    enum Criterio { TIEMPO, COSTO };

private:
    // ── Nodo interno del grafo ────────────────────────────────
    struct NodoGrafo {
        Sucursal              *sucursal;
        std::vector<Conexion>  conexiones; // aristas salientes

        explicit NodoGrafo(Sucursal *s) : sucursal(s) {}
    };

    std::vector<NodoGrafo*> nodos;

    // Auxiliar: busca el índice de una sucursal por ID — O(V)
    int indiceDe(const std::string &id) const;

    // Implementación de Dijkstra — O((V+E) log V)
    ResultadoRuta dijkstra(const std::string &origenId,
                           const std::string &destinoId,
                           Criterio criterio) const;

public:
    RedSucursales();
    ~RedSucursales();

    // ── Gestión del grafo ─────────────────────────────────────
    bool agregarSucursal(Sucursal *sucursal);
    bool eliminarSucursal(const std::string &id);
    bool agregarConexion(const std::string &origenId,
                         const std::string &destinoId,
                         double tiempo,
                         double costo,
                         bool bidireccional = true);
    bool eliminarConexion(const std::string &origenId,
                          const std::string &destinoId);

    // ── Consultas ─────────────────────────────────────────────
    Sucursal* buscarSucursal(const std::string &id) const;
    bool      existeConexion(const std::string &origenId,
                              const std::string &destinoId) const;
    int       contarSucursales() const;
    int       contarConexiones() const;

    std::vector<Sucursal*>  obtenerSucursales() const;
    std::vector<Conexion>   obtenerConexiones() const;
    std::vector<Conexion>   obtenerConexionesDe(const std::string &id) const;

    // ── Ruta óptima ───────────────────────────────────────────
    ResultadoRuta rutaOptima(const std::string &origenId,
                             const std::string &destinoId,
                             Criterio criterio = TIEMPO) const;

    // ETA total considerando pesos de aristas + tiempos de procesamiento
    double calcularETA(const ResultadoRuta &ruta,
                       Criterio criterio = TIEMPO) const;

    // ── Transferencia de producto ─────────────────────────────
    bool transferirProducto(const std::string &codigoBarra,
                            const std::string &origenId,
                            const std::string &destinoId,
                            Criterio criterio = TIEMPO);

    // ── Visualización ─────────────────────────────────────────
    void mostrarRed() const;
    bool generarDot(const std::string &rutaArchivo) const;

    // Para pintar la ruta resaltada en el .dot
    bool generarDotConRuta(const std::string &rutaArchivo,
                           const ResultadoRuta &ruta) const;
};

#endif // RED_SUCURSALES_H
