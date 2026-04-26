#include "MedidorRendimiento.h"
#include <iostream>
#include <iomanip>
#include <chrono>
#include <cmath>

/*
 * MedidorRendimiento.cpp
 * -----------------------
 * Usamos std::chrono::high_resolution_clock para medir con
 * la mayor precisión posible en el sistema.
 *
 * La idea del promedio M x N:
 *   - Hacemos M bloques de N búsquedas cada uno
 *   - Medimos el tiempo total de cada bloque
 *   - Promediamos los M tiempos → resultado más estable
 *   - Dividimos entre N para obtener el tiempo por búsqueda
 *
 * Esto reduce el impacto de picos ocasionales del OS
 * (interrupciones, cambios de contexto, etc.)
 */

MedidorRendimiento::MedidorRendimiento(ListaSimple *ls,
                                       ListaOrdenada *lo,
                                       ArbolAVL *avl)
    : listaSimple(ls), listaOrdenada(lo), arbolAVL(avl) {
}

// ============================================================
// Funciones de medición — una por estructura
// ============================================================

/*
 * Ejecuta M bloques de búsquedas sobre la lista simple.
 * Retorna el tiempo promedio por búsqueda en microsegundos.
 */
double MedidorRendimiento::medirListaSimple(const std::string nombres[],
                                            int cantidad) const {
    double totalUs = 0.0;

    for (int rep = 0; rep < M; rep++) {
        auto inicio = std::chrono::high_resolution_clock::now();

        for (int i = 0; i < N; i++) {
            // Rotamos los nombres para cubrir distintos casos en cada pasada
            listaSimple->buscarPorNombre(nombres[i % cantidad]);
        }

        auto fin = std::chrono::high_resolution_clock::now();
        auto duracion = std::chrono::duration_cast<std::chrono::microseconds>
                (fin - inicio);
        totalUs += static_cast<double>(duracion.count());
    }

    // Promediamos los M bloques y dividimos entre N para obtener µs/búsqueda
    return (totalUs / M) / N;
}

double MedidorRendimiento::medirListaOrdenada(const std::string nombres[],
                                              int cantidad) const {
    double totalUs = 0.0;

    for (int rep = 0; rep < M; rep++) {
        auto inicio = std::chrono::high_resolution_clock::now();

        for (int i = 0; i < N; i++) {
            listaOrdenada->buscarPorNombre(nombres[i % cantidad]);
        }

        auto fin = std::chrono::high_resolution_clock::now();
        auto duracion = std::chrono::duration_cast<std::chrono::microseconds>
                (fin - inicio);
        totalUs += static_cast<double>(duracion.count());
    }

    return (totalUs / M) / N;
}

double MedidorRendimiento::medirAVL(const std::string nombres[],
                                    int cantidad) const {
    double totalUs = 0.0;

    for (int rep = 0; rep < M; rep++) {
        auto inicio = std::chrono::high_resolution_clock::now();

        for (int i = 0; i < N; i++) {
            arbolAVL->buscar(nombres[i % cantidad]);
        }

        auto fin = std::chrono::high_resolution_clock::now();
        auto duracion = std::chrono::duration_cast<std::chrono::microseconds>
                (fin - inicio);
        totalUs += static_cast<double>(duracion.count());
    }

    return (totalUs / M) / N;
}

// ============================================================
// Impresión de la tabla
// ============================================================

void MedidorRendimiento::imprimirSeparador() const {
    std::cout << "+--------------------------+------------+-------------+------------+\n";
}

void MedidorRendimiento::imprimirEncabezado() const {
    imprimirSeparador();
    std::cout << "| Caso de prueba           | Lista      | Lista       | Arbol      |\n";
    std::cout << "|                          | Simple(us) | Ordenada(us)| AVL (us)   |\n";
    imprimirSeparador();
}

void MedidorRendimiento::imprimirFila(const std::string &caso,
                                      double usSimple,
                                      double usOrdenada,
                                      double usAVL) const {
    std::cout << "| " << std::left << std::setw(24) << caso << " | "
            << std::right << std::setw(10) << std::fixed
            << std::setprecision(3) << usSimple << " | "
            << std::setw(11) << usOrdenada << " | "
            << std::setw(10) << usAVL << " |\n";
}

// ============================================================
// Método principal
// ============================================================

void MedidorRendimiento::ejecutar(const std::string nombresExistentes[],
                                  int totalExistentes) const {
    std::cout << "\n=========================================\n";
    std::cout << "     Fase 7 - Medicion de Rendimiento   \n";
    std::cout << "=========================================\n";
    std::cout << "Metodologia: M=" << M << " bloques de N=" << N
            << " busquedas, promedio por busqueda en microsegundos (us)\n\n";

    // =============================================
    // CASO 1: Búsqueda exitosa aleatoria
    // Usamos nombres que sabemos que existen en el catálogo
    // =============================================
    double s1 = medirListaSimple(nombresExistentes, totalExistentes);
    double o1 = medirListaOrdenada(nombresExistentes, totalExistentes);
    double a1 = medirAVL(nombresExistentes, totalExistentes);

    // =============================================
    // CASO 2: Búsqueda fallida
    // Nombres que definitivamente no existen
    // =============================================
    const std::string noExistentes[] = {
        "Producto Fantasma Uno",
        "Articulo Inexistente",
        "Item No Registrado",
        "Zzzz Ultimo Posible",
        "Aaaa Primero Posible"
    };
    const int totalNoExistentes = 5;

    double s2 = medirListaSimple(noExistentes, totalNoExistentes);
    double o2 = medirListaOrdenada(noExistentes, totalNoExistentes);
    double a2 = medirAVL(noExistentes, totalNoExistentes);

    // =============================================
    // CASO 3: Extremos — primero y último
    // El primero es el mejor caso para la lista simple,
    // el último es el peor caso
    // =============================================
    // Tomamos el primero y último de los existentes que nos pasaron
    const std::string extremos[] = {
        nombresExistentes[0], // primero A-Z
        nombresExistentes[totalExistentes - 1] // último A-Z
    };
    const int totalExtremos = 2;

    double s3 = medirListaSimple(extremos, totalExtremos);
    double o3 = medirListaOrdenada(extremos, totalExtremos);
    double a3 = medirAVL(extremos, totalExtremos);

    // =============================================
    // CASO 4: Solo el PEOR caso (último elemento)
    // Muestra bien la diferencia O(n) vs O(log n)
    // =============================================
    const std::string peorCaso[] = {nombresExistentes[totalExistentes - 1]};

    double s4 = medirListaSimple(peorCaso, 1);
    double o4 = medirListaOrdenada(peorCaso, 1);
    double a4 = medirAVL(peorCaso, 1);

    // =============================================
    // IMPRIMIMOS LA TABLA
    // =============================================
    imprimirEncabezado();
    imprimirFila("Exitosa aleatoria", s1, o1, a1);
    imprimirSeparador();
    imprimirFila("Fallida", s2, o2, a2);
    imprimirSeparador();
    imprimirFila("Extremos (mix)", s3, o3, a3);
    imprimirSeparador();
    imprimirFila("Peor caso (ultimo)", s4, o4, a4);
    imprimirSeparador();

    // =============================================
    // ANÁLISIS: factores de aceleración
    // =============================================
    std::cout << "\n--- Analisis de factores de aceleracion ---\n";
    std::cout << std::fixed << std::setprecision(1);

    if (a1 > 0) {
        std::cout << "Busqueda exitosa  -> Lista simple es "
                << (s1 / a1) << "x mas lenta que AVL\n";
        std::cout << "                     Lista ordenada es "
                << (o1 / a1) << "x mas lenta que AVL\n";
    }
    if (a2 > 0) {
        std::cout << "Busqueda fallida  -> Lista simple es "
                << (s2 / a2) << "x mas lenta que AVL\n";
        std::cout << "                     Lista ordenada es "
                << (o2 / a2) << "x mas lenta que AVL\n";
    }
    if (a4 > 0) {
        std::cout << "Peor caso         -> Lista simple es "
                << (s4 / a4) << "x mas lenta que AVL\n";
        std::cout << "                     Lista ordenada es "
                << (o4 / a4) << "x mas lenta que AVL\n";
    }

    // =============================================
    // COMPLEJIDADES TEÓRICAS
    // =============================================
    std::cout << "\n--- Complejidades teoricas vs empiricas ---\n";
    std::cout << "Estructura      | Teorico   | Observado\n";
    std::cout << "----------------+-----------+----------\n";
    std::cout << "Lista simple    | O(n)      | " << std::setprecision(3)
            << s1 << " us promedio\n";
    std::cout << "Lista ordenada  | O(n)      | " << o1 << " us promedio\n";
    std::cout << "Arbol AVL       | O(log n)  | " << a1 << " us promedio\n";
    std::cout << "\nNota: con n=" << totalExistentes
            << " elementos, log2(n) ≈ "
            << std::setprecision(1)
            << (std::log2(totalExistentes))
            << " niveles de profundidad en el AVL.\n";
}
