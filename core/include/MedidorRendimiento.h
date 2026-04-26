#ifndef PROYECTO_EDD_MEDIDORRENDIMIENTO_H
#define PROYECTO_EDD_MEDIDORRENDIMIENTO_H

#include "Catalogo.h"
#include "ListaSimple.h"
#include "ListaOrdenada.h"
#include "ArbolAVL.h"
#include <string>

/*
 * MedidorRendimiento.h
 * ---------------------
 * Compara los tiempos de búsqueda por nombre entre:
 *   - Lista simple    (búsqueda secuencial O(n))
 *   - Lista ordenada  (búsqueda secuencial con corte anticipado O(n))
 *   - Árbol AVL       (búsqueda binaria O(log n))
 *
 * Metodología (según el proyecto tenemos):
 *   - N = 20 consultas por tipo de caso
 *   - M = 5 repeticiones del bloque de N consultas
 *   - Se promedian los M bloques para obtener el tiempo final
 *
 * Casos evaluados:
 *   1. Búsqueda exitosa aleatoria  — elemento existe
 *   2. Búsqueda fallida            — elemento no existe
 *   3. Búsqueda en extremos        — primero y último alfabéticamente
 *
 * Los resultados los mostramos en una tabla en consola (µs y ms).
 */

class MedidorRendimiento {
private:
    // Referencia a las estructuras que vamos a medir
    ListaSimple *listaSimple;
    ListaOrdenada *listaOrdenada;
    ArbolAVL *arbolAVL;

    static const int N = 20; // consultas por bloque
    static const int M = 5; // repeticiones del bloque

    // Mide el tiempo promedio de N búsquedas en la lista simple (µs)
    double medirListaSimple(const std::string nombres[], int cantidad) const;

    // Mide el tiempo promedio de N búsquedas en la lista ordenada (µs)
    double medirListaOrdenada(const std::string nombres[], int cantidad) const;

    // Mide el tiempo promedio de N búsquedas en el AVL (µs)
    double medirAVL(const std::string nombres[], int cantidad) const;

    // Imprime una fila de la tabla de resultados
    void imprimirFila(const std::string &caso, double usSimple,
                      double usOrdenada, double usAVL) const;

    // Imprime el encabezado de la tabla
    void imprimirEncabezado() const;

    void imprimirSeparador() const;

public:
    // Recibe punteros a las estructuras ya cargadas con datos
    MedidorRendimiento(ListaSimple *ls, ListaOrdenada *lo, ArbolAVL *avl);

    // Ejecuta todos los benchmarks y muestra la tabla comparativa
    void ejecutar(const std::string nombresExistentes[],
                  int totalExistentes) const;
};

#endif //PROYECTO_EDD_MEDIDORRENDIMIENTO_H
