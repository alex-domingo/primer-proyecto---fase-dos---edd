#include "Producto.h"
#include <iostream>
#include <iomanip>

// Constructor por defecto
Producto::Producto()
    : nombre(""), codigoBarra(""), categoria(""),
    fechaCaducidad(""), marca(""), precio(0.0), stock(0),
    sucursalId(""), estado("Disponible"),
    sucursalEntradaId(""), sucursalSalidaId("") {}

// Constructor Fase 1 — compatible con todo el código existente
Producto::Producto(const std::string &nombre,
                   const std::string &codigoBarra,
                   const std::string &categoria,
                   const std::string &fechaCaducidad,
                   const std::string &marca,
                   double precio,
                   int stock)
    : nombre(nombre), codigoBarra(codigoBarra), categoria(categoria),
    fechaCaducidad(fechaCaducidad), marca(marca),
    precio(precio), stock(stock),
    sucursalId(""), estado("Disponible"),
    sucursalEntradaId(""), sucursalSalidaId("") {}

// Constructor Fase 2 — incluye sucursal y estado
Producto::Producto(const std::string &nombre,
                   const std::string &codigoBarra,
                   const std::string &categoria,
                   const std::string &fechaCaducidad,
                   const std::string &marca,
                   double precio,
                   int stock,
                   const std::string &sucursalId,
                   const std::string &estado)
    : nombre(nombre), codigoBarra(codigoBarra), categoria(categoria),
    fechaCaducidad(fechaCaducidad), marca(marca),
    precio(precio), stock(stock),
    sucursalId(sucursalId), estado(estado),
    sucursalEntradaId(sucursalId), sucursalSalidaId(sucursalId) {}

// Constructor Fase 2 (lotes/hilos) — entrada y salida explícitas.
// El producto reside en la sucursal de ENTRADA al cargarse.
Producto::Producto(const std::string &nombre,
                   const std::string &codigoBarra,
                   const std::string &categoria,
                   const std::string &fechaCaducidad,
                   const std::string &marca,
                   double precio,
                   int stock,
                   const std::string &sucursalEntradaId,
                   const std::string &sucursalSalidaId,
                   const std::string &estado)
    : nombre(nombre), codigoBarra(codigoBarra), categoria(categoria),
    fechaCaducidad(fechaCaducidad), marca(marca),
    precio(precio), stock(stock),
    sucursalId(sucursalEntradaId), estado(estado),
    sucursalEntradaId(sucursalEntradaId),
    sucursalSalidaId(sucursalSalidaId) {}

void Producto::mostrar() const {
    std::cout << "------------------------------\n";
    std::cout << "Nombre       : " << nombre        << "\n";
    std::cout << "Codigo Barra : " << codigoBarra   << "\n";
    std::cout << "Categoria    : " << categoria     << "\n";
    std::cout << "Caducidad    : " << fechaCaducidad << "\n";
    std::cout << "Marca        : " << marca         << "\n";
    std::cout << "Precio       : Q" << std::fixed
              << std::setprecision(2) << precio     << "\n";
    std::cout << "Stock        : " << stock << " unidades\n";
    if (!sucursalId.empty())
        std::cout << "Sucursal     : " << sucursalId << "\n";
    std::cout << "Estado       : " << estado        << "\n";
    std::cout << "------------------------------\n";
}

bool Producto::operator<(const Producto &otro) const {
    return nombre < otro.nombre;
}

bool Producto::operator==(const Producto &otro) const {
    return codigoBarra == otro.codigoBarra;
}
