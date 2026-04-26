#include "Catalogo.h"
#include <iostream>

/*
 * Catalogo.cpp — Fase 2
 * ----------------------
 * Integra la TablaHash como sexta estructura.
 *
 * Orden de inserción atómica (rollback si algo falla):
 *   1. listaSimple    → siempre OK
 *   2. listaOrdenada  → siempre OK
 *   3. arbolAVL       → falla si nombre duplicado
 *                        rollback: eliminar de listas 1-2
 *   4. tablaHash      → falla si código duplicado
 *                        rollback: eliminar de listas + AVL
 *   5. arbolB         → siempre OK (permite fechas duplicadas)
 *   6. arbolBPlus     → siempre OK (permite categorías duplicadas)
 */

Catalogo::Catalogo() : totalProductos(0) {
    listaSimple   = new ListaSimple();
    listaOrdenada = new ListaOrdenada();
    arbolAVL      = new ArbolAVL();
    arbolB        = new ArbolB();
    arbolBPlus    = new ArbolBPlus();
    tablaHash     = new TablaHash();
}

Catalogo::~Catalogo() {
    delete listaSimple;
    delete listaOrdenada;
    delete arbolAVL;
    delete arbolB;
    delete arbolBPlus;
    delete tablaHash;
}

bool Catalogo::existeCodigo(const std::string &codigoBarra) const {
    return tablaHash->buscar(codigoBarra) != nullptr;
}

// ============================================================
// Inserción atómica con rollback
// ============================================================

bool Catalogo::agregarProducto(const Producto &producto) {
    // Validaciones previas
    if (producto.nombre.empty()        || producto.codigoBarra.empty() ||
        producto.categoria.empty()     || producto.fechaCaducidad.empty()) {
        std::cout << "[Catalogo] ERROR: campos obligatorios vacios.\n";
        return false;
    }
    if (producto.precio < 0 || producto.stock < 0) {
        std::cout << "[Catalogo] ERROR: precio o stock negativos.\n";
        return false;
    }
    if (existeCodigo(producto.codigoBarra)) {
        std::cout << "[Catalogo] ERROR: codigo '" << producto.codigoBarra
                  << "' ya existe.\n";
        return false;
    }

    // Paso 1 y 2: listas (nunca fallan)
    listaSimple->insertar(producto);
    listaOrdenada->insertar(producto);

    // Paso 3: AVL — falla si nombre duplicado
    int tamAVLAntes = arbolAVL->obtenerTamano();
    arbolAVL->insertar(producto);
    if (arbolAVL->obtenerTamano() == tamAVLAntes) {
        listaSimple->eliminar(producto.codigoBarra);
        listaOrdenada->eliminar(producto.codigoBarra);
        std::cout << "[Catalogo] ERROR: nombre '" << producto.nombre
                  << "' ya existe. Rollback aplicado.\n";
        return false;
    }

    // Paso 4: TablaHash — falla si código duplicado (ya verificamos arriba,
    // pero insertar() también devuelve false como segunda línea de defensa)
    if (!tablaHash->insertar(producto)) {
        listaSimple->eliminar(producto.codigoBarra);
        listaOrdenada->eliminar(producto.codigoBarra);
        arbolAVL->eliminar(producto.nombre);
        std::cout << "[Catalogo] ERROR: codigo '" << producto.codigoBarra
                  << "' duplicado en Hash. Rollback aplicado.\n";
        return false;
    }

    // Pasos 5 y 6: B y B+ (no fallan por duplicado)
    arbolB->insertar(producto);
    arbolBPlus->insertar(producto);

    totalProductos++;
    return true;
}

// ============================================================
// Eliminación propagada
// ============================================================

bool Catalogo::eliminarProducto(const std::string &nombre,
                                const std::string &codigoBarra,
                                const std::string &categoria,
                                const std::string &fechaCaducidad) {
    if (!existeCodigo(codigoBarra)) {
        std::cout << "[Catalogo] ERROR: producto no encontrado.\n";
        return false;
    }
    listaSimple->eliminar(codigoBarra);
    listaOrdenada->eliminar(codigoBarra);
    arbolAVL->eliminar(nombre);
    tablaHash->eliminar(codigoBarra);
    arbolB->eliminar(fechaCaducidad);
    arbolBPlus->eliminar(categoria, codigoBarra);
    totalProductos--;
    return true;
}

// ============================================================
// Búsquedas
// ============================================================

Producto *Catalogo::buscarPorNombre(const std::string &nombre) {
    Producto *r = arbolAVL->buscar(nombre);
    if (!r) std::cout << "[Catalogo] Producto '" << nombre << "' no encontrado.\n";
    return r;
}

Producto *Catalogo::buscarPorCodigo(const std::string &codigoBarra) {
    Producto *r = tablaHash->buscar(codigoBarra);
    if (!r) std::cout << "[Catalogo] Codigo '" << codigoBarra << "' no encontrado.\n";
    return r;
}

void Catalogo::buscarPorCategoria(const std::string &categoria) {
    arbolBPlus->buscarPorCategoria(categoria);
}

void Catalogo::buscarPorRangoFecha(const std::string &inicio,
                                   const std::string &fin) {
    arbolB->buscarRango(inicio, fin);
}

// ============================================================
// Listados
// ============================================================

void Catalogo::listarPorNombre() const {
    std::cout << "=== Listado por nombre (AVL in-order) ===\n";
    arbolAVL->listarEnOrden();
}

void Catalogo::listarSimple() const {
    std::cout << "=== Listado lista simple (orden insercion) ===\n";
    listaSimple->listar();
}

void Catalogo::listarOrdenado() const {
    std::cout << "=== Listado lista ordenada (alfabetico) ===\n";
    listaOrdenada->listar();
}

int  Catalogo::contarProductos() const { return totalProductos; }
bool Catalogo::estaVacio()        const { return totalProductos == 0; }
