#ifndef PROYECTO_EDD_PRODUCTO_H
#define PROYECTO_EDD_PRODUCTO_H

#include <string>

/*
 * Producto.h
 * ----------
 * Entidad base del sistema. Fase 2 agrega:
 *   - sucursalId : a qué sucursal pertenece este producto
 *   - estado     : "Disponible", "EnTransito" o "Agotado"
 *
 * Todos los demás campos son idénticos a Fase 1.
 */

class Producto {
public:
    std::string nombre;
    std::string codigoBarra;   // exactamente 10 caracteres (validado en carga)
    std::string categoria;
    std::string fechaCaducidad; // formato ISO: "YYYY-MM-DD"
    std::string marca;
    double      precio;
    int         stock;

    // ── Fase 2 ──────────────────────────────────────────────
    std::string sucursalId;    // ID de la sucursal propietaria
    std::string estado;        // "Disponible" | "EnTransito" | "Agotado"

    // Constructor por defecto
    Producto();

    // Constructor Fase 1 — sucursalId vacío, estado "Disponible"
    Producto(const std::string &nombre,
             const std::string &codigoBarra,
             const std::string &categoria,
             const std::string &fechaCaducidad,
             const std::string &marca,
             double precio,
             int stock);

    // Constructor Fase 2 — incluye sucursalId y estado
    Producto(const std::string &nombre,
             const std::string &codigoBarra,
             const std::string &categoria,
             const std::string &fechaCaducidad,
             const std::string &marca,
             double precio,
             int stock,
             const std::string &sucursalId,
             const std::string &estado = "Disponible");

    void mostrar() const;

    bool operator<(const Producto &otro) const;
    bool operator==(const Producto &otro) const;
};

#endif // PROYECTO_EDD_PRODUCTO_H
