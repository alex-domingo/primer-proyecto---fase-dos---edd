#ifndef OPERACION_H
#define OPERACION_H

#include "Producto.h"
#include <string>

/*
 * Operacion.h
 * ------------
 * Registro de una operación realizada sobre el catálogo.
 * Se apila en PilaOperaciones para permitir deshacer (rollback).
 *
 * Tipos de operación:
 *   AGREGAR   — se deshace eliminando el producto
 *   ELIMINAR  — se deshace re-insertando el producto
 *   TRANSFERIR— se deshace revirtiendo el estado y sucursal
 */

struct Operacion {
    enum Tipo { AGREGAR, ELIMINAR, TRANSFERIR };

    Tipo        tipo;
    Producto    producto;        // copia del producto afectado
    std::string sucursalId;      // sucursal donde ocurrió
    std::string sucursalDestino; // solo para TRANSFERIR

    Operacion(Tipo tipo,
              const Producto &producto,
              const std::string &sucursalId,
              const std::string &sucursalDestino = "")
        : tipo(tipo)
        , producto(producto)
        , sucursalId(sucursalId)
        , sucursalDestino(sucursalDestino)
    {}
};

#endif // OPERACION_H
