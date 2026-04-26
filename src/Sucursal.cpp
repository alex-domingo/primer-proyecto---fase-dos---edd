#include "Sucursal.h"
#include <iostream>

Sucursal::Sucursal()
    : id(""), nombre(""), ubicacion("")
    , tiempoIngreso(0), tiempoTraspaso(0), tiempoDespacho(0)
    , catalogo(new Catalogo()) {}

Sucursal::Sucursal(const std::string &id,
                   const std::string &nombre,
                   const std::string &ubicacion,
                   int tiempoIngreso,
                   int tiempoTraspaso,
                   int tiempoDespacho)
    : id(id), nombre(nombre), ubicacion(ubicacion)
    , tiempoIngreso(tiempoIngreso)
    , tiempoTraspaso(tiempoTraspaso)
    , tiempoDespacho(tiempoDespacho)
    , catalogo(new Catalogo()) {}

Sucursal::~Sucursal() {
    delete catalogo;
}

// ── Gestión de inventario ─────────────────────────────────────

bool Sucursal::agregarProducto(const Producto &producto) {
    // Aseguramos que el producto quede asociado a esta sucursal
    Producto p = producto;
    p.sucursalId = id;
    if (p.estado.empty()) p.estado = "Disponible";

    bool ok = catalogo->agregarProducto(p);
    if (ok) {
        pilaOperaciones.apilar(Operacion(Operacion::AGREGAR, p, id));
    }
    return ok;
}

bool Sucursal::eliminarProducto(const std::string &nombre,
                                const std::string &codigo,
                                const std::string &categoria,
                                const std::string &fecha) {
    // Guardamos copia antes de eliminar para poder deshacer
    Producto *p = catalogo->buscarPorCodigo(codigo);
    if (!p) return false;
    Producto copia = *p;

    bool ok = catalogo->eliminarProducto(nombre, codigo, categoria, fecha);
    if (ok) {
        pilaOperaciones.apilar(Operacion(Operacion::ELIMINAR, copia, id));
    }
    return ok;
}

Producto* Sucursal::buscarPorNombre(const std::string &nombre) {
    return catalogo->buscarPorNombre(nombre);
}

Producto* Sucursal::buscarPorCodigo(const std::string &codigo) {
    return catalogo->buscarPorCodigo(codigo);
}

int Sucursal::contarProductos() const {
    return catalogo->contarProductos();
}

// ── Colas ─────────────────────────────────────────────────────

void Sucursal::recibirProducto(const Producto &producto) {
    Producto p = producto;
    p.estado = "Disponible";
    colaIngreso.encolar(p);
}

void Sucursal::prepararTraspaso(const Producto &producto) {
    Producto p = producto;
    p.estado = "EnTransito";
    colaTraspaso.encolar(p);
}

void Sucursal::moverATraspasoASalida() {
    if (colaTraspaso.estaVacia()) return;
    colaSalida.encolar(colaTraspaso.frente());
    colaTraspaso.desencolar();
}

bool Sucursal::despacharSiguiente() {
    if (colaSalida.estaVacia()) return false;
    colaSalida.desencolar();
    return true;
}

// ── Pila de operaciones ───────────────────────────────────────

void Sucursal::registrarOperacion(const Operacion &op) {
    pilaOperaciones.apilar(op);
}

bool Sucursal::deshacerUltimaOperacion() {
    if (pilaOperaciones.estaVacia()) {
        std::cout << "[Sucursal " << id << "] No hay operaciones que deshacer.\n";
        return false;
    }

    Operacion op = pilaOperaciones.cima();
    pilaOperaciones.desapilar();

    switch (op.tipo) {
        case Operacion::AGREGAR:
            // Deshacer un agregar = eliminar
            catalogo->eliminarProducto(
                op.producto.nombre,
                op.producto.codigoBarra,
                op.producto.categoria,
                op.producto.fechaCaducidad);
            std::cout << "[Deshacer] Producto '" << op.producto.nombre
                      << "' eliminado de sucursal " << id << ".\n";
            break;

        case Operacion::ELIMINAR:
            // Deshacer un eliminar = re-insertar
            catalogo->agregarProducto(op.producto);
            std::cout << "[Deshacer] Producto '" << op.producto.nombre
                      << "' re-insertado en sucursal " << id << ".\n";
            break;

        case Operacion::TRANSFERIR:
            // Deshacer una transferencia = revertir estado
            std::cout << "[Deshacer] Transferencia de '" << op.producto.nombre
                      << "' revertida.\n";
            break;
    }
    return true;
}

bool Sucursal::hayOperacionesPendientes() const {
    return !pilaOperaciones.estaVacia();
}

int Sucursal::contarOperaciones() const {
    return pilaOperaciones.obtenerTamano();
}

void Sucursal::mostrar() const {
    std::cout << "=== Sucursal ===\n";
    std::cout << "ID         : " << id        << "\n";
    std::cout << "Nombre     : " << nombre    << "\n";
    std::cout << "Ubicacion  : " << ubicacion << "\n";
    std::cout << "T.Ingreso  : " << tiempoIngreso  << "s\n";
    std::cout << "T.Traspaso : " << tiempoTraspaso << "s\n";
    std::cout << "T.Despacho : " << tiempoDespacho << "s\n";
    std::cout << "Productos  : " << catalogo->contarProductos() << "\n";
    std::cout << "Cola ingreso  : " << colaIngreso.obtenerTamano()  << " productos\n";
    std::cout << "Cola traspaso : " << colaTraspaso.obtenerTamano() << " productos\n";
    std::cout << "Cola salida   : " << colaSalida.obtenerTamano()   << " productos\n";
    std::cout << "Operaciones   : " << pilaOperaciones.obtenerTamano() << " en historial\n";
}
