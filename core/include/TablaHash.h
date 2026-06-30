#ifndef TABLA_HASH_H
#define TABLA_HASH_H

#include "NodoHash.h"
#include <string>

/*
 * TablaHash.h
 * -----------
 * Tabla hash con encadenamiento separado (chaining).
 * La clave es el código de barra de cada producto (único).
 *
 * Funcionamiento:
 *   1. Tomamos el código de barra (string)
 *   2. Lo pasamos por la función hash -> obtenemos un índice
 *   3. En ese índice del arreglo hay una lista enlazada
 *   4. Agregamos/buscamos/eliminamos en esa lista
 *
 * La función hash que usamos es djb2, una función clásica
 * y simple que distribuye bien strings de longitud variable.
 *
 * Complejidad (con factor de carga razonable):
 *   insertar  -> O(1) amortizado
 *   buscar    -> O(1) amortizado
 *   eliminar  -> O(1) amortizado
 *   peor caso -> O(n) si todas las claves colisionan
 *
 * Capacidad inicial: 101 buckets (número primo para mejor distribución)
 */

class TablaHash {
private:
    static const int CAPACIDAD = 101; // primo para reducir colisiones

    NodoHash *tabla[CAPACIDAD]; // arreglo de punteros a listas enlazadas
    int       tamano;           // cantidad total de productos almacenados
    int       colisiones;       // contador para análisis de rendimiento

    // Función hash djb2 — convierte el string en un índice válido
    int funcionHash(const std::string &clave) const;

public:
    TablaHash();
    ~TablaHash();

    // Inserta un producto usando su código de barra como clave — O(1) prom.
    bool insertar(const Producto &producto);

    // Busca por código de barra — O(1) prom.
    Producto *buscar(const std::string &codigoBarra);

    // Elimina por código de barra — O(1) prom.
    bool eliminar(const std::string &codigoBarra);

    // Muestra estadísticas: buckets usados, colisiones, factor de carga
    void mostrarEstadisticas() const;

    // Lista todos los productos almacenados
    void listar() const;

    int  obtenerTamano() const;
    bool estaVacia() const;

    // ── Acceso de solo lectura para visualización ─────────────
    int obtenerCapacidad()  const { return CAPACIDAD; }
    int obtenerColisiones() const { return colisiones; }
    // Devuelve la cabeza de la lista enlazada del bucket i (o nullptr)
    NodoHash* obtenerBucket(int i) const {
        if (i < 0 || i >= CAPACIDAD) return nullptr;
        return tabla[i];
    }
    // Factor de carga = elementos / buckets
    double factorCarga() const {
        return CAPACIDAD > 0 ? (double)tamano / CAPACIDAD : 0.0;
    }
};

#endif // TABLA_HASH_H
