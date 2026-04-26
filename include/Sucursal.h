#ifndef SUCURSAL_H
#define SUCURSAL_H

#include "Catalogo.h"
#include "Cola.h"
#include "Pila.h"
#include "Operacion.h"
#include "Producto.h"
#include <string>

/*
 * Sucursal.h
 * -----------
 * Representa una sucursal de la red.
 * Cada sucursal tiene:
 *   - Su propio catálogo independiente (6 estructuras)
 *   - Tres colas de productos (ingreso, traspaso, salida)
 *   - Una pila de operaciones para rollback/deshacer
 *   - Parámetros de tiempo que controlan la simulación
 *
 * Parámetros de tiempo (en segundos):
 *   tiempoIngreso  : tiempo en procesar la llegada de un producto
 *   tiempoTraspaso : tiempo en preparar un producto para reenvío
 *   tiempoDespacho : intervalo entre cada envío que puede hacer
 */

class Sucursal {
private:
    std::string id;
    std::string nombre;
    std::string ubicacion;

    // Parámetros de tiempo de la simulación
    int tiempoIngreso;    // segundos para procesar llegada
    int tiempoTraspaso;   // segundos para preparar reenvío
    int tiempoDespacho;   // intervalo entre despachos

    // Catálogo propio — 6 estructuras sincronizadas
    Catalogo *catalogo;

    // Colas de productos
    Cola<Producto> colaIngreso;    // productos recién recibidos
    Cola<Producto> colaTraspaso;   // en preparación para reenvío
    Cola<Producto> colaSalida;     // listos para despachar

    // Pila de operaciones para deshacer
    Pila<Operacion> pilaOperaciones;

public:
    Sucursal();
    Sucursal(const std::string &id,
             const std::string &nombre,
             const std::string &ubicacion,
             int tiempoIngreso,
             int tiempoTraspaso,
             int tiempoDespacho);
    ~Sucursal();

    // ── Getters ──────────────────────────────────────────────
    std::string getId()        const { return id;            }
    std::string getNombre()    const { return nombre;        }
    std::string getUbicacion() const { return ubicacion;     }
    int getTiempoIngreso()     const { return tiempoIngreso; }
    int getTiempoTraspaso()    const { return tiempoTraspaso;}
    int getTiempoDespacho()    const { return tiempoDespacho;}
    Catalogo*   getCatalogo()  const { return catalogo;      }

    // ── Setters (para edición desde la GUI) ──────────────────
    void setNombre(const std::string &n)    { nombre = n;    }
    void setUbicacion(const std::string &u) { ubicacion = u; }
    void setTiempoIngreso(int t)   { tiempoIngreso = t;   }
    void setTiempoTraspaso(int t)  { tiempoTraspaso = t;  }
    void setTiempoDespacho(int t)  { tiempoDespacho = t;  }

    // ── Gestión de inventario ─────────────────────────────────
    bool agregarProducto(const Producto &producto);
    bool eliminarProducto(const std::string &nombre,
                          const std::string &codigo,
                          const std::string &categoria,
                          const std::string &fecha);
    Producto* buscarPorNombre(const std::string &nombre);
    Producto* buscarPorCodigo(const std::string &codigo);
    int contarProductos() const;

    // ── Colas ─────────────────────────────────────────────────
    void recibirProducto(const Producto &producto);  // → colaIngreso
    void prepararTraspaso(const Producto &producto); // → colaTraspaso
    void moverATraspasoASalida();                    // colaTraspaso → colaSalida
    bool despacharSiguiente();                       // pop colaSalida

    Cola<Producto>& getColaIngreso()  { return colaIngreso;  }
    Cola<Producto>& getColaTraspaso() { return colaTraspaso; }
    Cola<Producto>& getColaSalida()   { return colaSalida;   }

    // ── Pila de operaciones (rollback) ────────────────────────
    void registrarOperacion(const Operacion &op);
    bool deshacerUltimaOperacion();
    bool hayOperacionesPendientes() const;
    int  contarOperaciones() const;

    void mostrar() const;
};

#endif // SUCURSAL_H
