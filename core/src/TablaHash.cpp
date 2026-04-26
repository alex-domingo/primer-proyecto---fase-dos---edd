#include "TablaHash.h"
#include <iostream>
#include <iomanip>

/*
 * TablaHash.cpp
 * -------------
 * Implementación de la tabla hash con encadenamiento separado.
 *
 * Lo más importante aquí es la función hash: toma cada carácter
 * del string y lo mezcla con el valor acumulado usando operaciones
 * de bits. Esto produce una distribución bastante uniforme.
 *
 * El módulo al final asegura que el resultado siempre caiga
 * dentro del rango válido de índices [0, CAPACIDAD-1].
 */

// ============================================================
// Constructor y destructor
// ============================================================

TablaHash::TablaHash() : tamano(0), colisiones(0) {
    // Inicializamos cada bucket en nullptr (lista vacía)
    for (int i = 0; i < CAPACIDAD; i++) {
        tabla[i] = nullptr;
    }
}

TablaHash::~TablaHash() {
    // Liberamos cada lista enlazada de cada bucket
    for (int i = 0; i < CAPACIDAD; i++) {
        NodoHash *actual = tabla[i];
        while (actual != nullptr) {
            NodoHash *siguiente = actual->siguiente;
            delete actual;
            actual = siguiente;
        }
        tabla[i] = nullptr;
    }
}

// ============================================================
// Función hash djb2
// ============================================================

/*
 * djb2: hash = hash * 33 + c  (para cada carácter c del string)
 * El truco del 5381 y el << 5 es equivalente a multiplicar por 33,
 * pero más rápido porque usa operaciones de bits en vez de multiplicación.
 *
 * El resultado lo llevamos a unsigned para evitar índices negativos,
 * y luego aplicamos módulo para quedarnos dentro del arreglo.
 */
int TablaHash::funcionHash(const std::string &clave) const {
    unsigned long hash = 5381;
    for (char c : clave) {
        // hash = hash * 33 + c
        hash = ((hash << 5) + hash) + static_cast<unsigned char>(c);
    }
    return static_cast<int>(hash % CAPACIDAD);
}

// ============================================================
// Inserción
// ============================================================

bool TablaHash::insertar(const Producto &producto) {
    int indice = funcionHash(producto.codigoBarra);

    // Revisamos si ya existe ese código de barra en el bucket
    // (no permitimos duplicados porque el código es clave única)
    NodoHash *actual = tabla[indice];
    while (actual != nullptr) {
        if (actual->dato.codigoBarra == producto.codigoBarra) {
            return false; // duplicado, rechazamos
        }
        actual = actual->siguiente;
    }

    // Si el bucket ya tenía algo, esta es una colisión
    if (tabla[indice] != nullptr) {
        colisiones++;
    }

    // Insertamos al frente de la lista del bucket — O(1)
    NodoHash *nuevo    = new NodoHash(producto);
    nuevo->siguiente   = tabla[indice];
    tabla[indice]      = nuevo;
    tamano++;
    return true;
}

// ============================================================
// Búsqueda
// ============================================================

Producto *TablaHash::buscar(const std::string &codigoBarra) {
    int indice = funcionHash(codigoBarra);

    // Recorremos solo la lista del bucket correspondiente
    NodoHash *actual = tabla[indice];
    while (actual != nullptr) {
        if (actual->dato.codigoBarra == codigoBarra) {
            return &actual->dato;
        }
        actual = actual->siguiente;
    }
    return nullptr; // no encontrado
}

// ============================================================
// Eliminación
// ============================================================

bool TablaHash::eliminar(const std::string &codigoBarra) {
    int indice = funcionHash(codigoBarra);

    if (tabla[indice] == nullptr) {
        return false; // bucket vacío
    }

    // Caso especial: el nodo a eliminar es el primero del bucket
    if (tabla[indice]->dato.codigoBarra == codigoBarra) {
        NodoHash *aBorrar = tabla[indice];
        tabla[indice]     = tabla[indice]->siguiente;
        delete aBorrar;
        tamano--;
        return true;
    }

    // Caso general: buscamos el nodo anterior al que queremos borrar
    NodoHash *anterior = tabla[indice];
    while (anterior->siguiente != nullptr) {
        if (anterior->siguiente->dato.codigoBarra == codigoBarra) {
            NodoHash *aBorrar    = anterior->siguiente;
            anterior->siguiente  = aBorrar->siguiente;
            delete aBorrar;
            tamano--;
            return true;
        }
        anterior = anterior->siguiente;
    }

    return false; // no estaba en la lista
}

// ============================================================
// Estadísticas y listado
// ============================================================

void TablaHash::mostrarEstadisticas() const {
    int bucketsUsados = 0;
    int maxCadena     = 0; // cadena más larga (peor caso de búsqueda)

    for (int i = 0; i < CAPACIDAD; i++) {
        if (tabla[i] != nullptr) {
            bucketsUsados++;
            // Contamos cuántos nodos tiene este bucket
            int longCadena  = 0;
            NodoHash *actual = tabla[i];
            while (actual != nullptr) {
                longCadena++;
                actual = actual->siguiente;
            }
            if (longCadena > maxCadena) maxCadena = longCadena;
        }
    }

    double factorCarga = (CAPACIDAD > 0)
                         ? static_cast<double>(tamano) / CAPACIDAD
                         : 0.0;

    std::cout << "---- Estadisticas de la Tabla Hash ----\n";
    std::cout << "Capacidad (buckets) : " << CAPACIDAD      << "\n";
    std::cout << "Productos guardados : " << tamano         << "\n";
    std::cout << "Buckets usados      : " << bucketsUsados  << "\n";
    std::cout << "Colisiones totales  : " << colisiones     << "\n";
    std::cout << "Cadena mas larga    : " << maxCadena      << " nodos\n";
    std::cout << "Factor de carga     : "
              << std::fixed << std::setprecision(3) << factorCarga << "\n";
    std::cout << "---------------------------------------\n";
}

void TablaHash::listar() const {
    if (tamano == 0) {
        std::cout << "[TablaHash] La tabla esta vacia.\n";
        return;
    }

    int contador = 1;
    for (int i = 0; i < CAPACIDAD; i++) {
        NodoHash *actual = tabla[i];
        while (actual != nullptr) {
            std::cout << contador++ << ". [bucket " << i << "] ";
            actual->dato.mostrar();
            actual = actual->siguiente;
        }
    }
}

int TablaHash::obtenerTamano() const {
    return tamano;
}

bool TablaHash::estaVacia() const {
    return tamano == 0;
}
