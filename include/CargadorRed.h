#ifndef CARGADOR_RED_H
#define CARGADOR_RED_H

#include "RedSucursales.h"
#include <string>

/*
 * CargadorRed.h
 * --------------
 * Carga los tres archivos CSV que definen la red de sucursales:
 *
 *   1. sucursales.csv
 *      "ID","Nombre","Ubicacion","TiempoIngreso","TiempoTraspaso","TiempoDespacho"
 *
 *   2. conexiones.csv
 *      "OrigenID","DestinoID","Tiempo","Costo","Bidireccional"
 *
 *   3. productos_fase2.csv
 *      "SucursalID","Nombre","CodigoBarra","Categoria",
 *      "FechaCaducidad","Marca","Precio","Stock","Estado"
 *
 * El orden de carga es obligatorio: primero sucursales, luego
 * conexiones, luego productos — las conexiones necesitan que las
 * sucursales ya existan, y los productos necesitan la sucursal destino.
 *
 * Los errores se registran en data/errors_red.log.
 */

class CargadorRed {
private:
    std::string rutaLog;

    void        loggearError(int linea, const std::string &motivo,
                             const std::string &lineaOriginal) const;
    std::string limpiarComillas(const std::string &campo) const;
    bool        parsearLinea(const std::string &linea,
                             std::string campos[],
                             int numEsperados) const;
    bool        esEnteroValido(const std::string &texto) const;
    bool        esNumeroValido(const std::string &texto) const;

public:
    explicit CargadorRed(const std::string &rutaLog = "data/errors_red.log");

    // Carga sucursales.csv — debe llamarse primero
    int cargarSucursales(const std::string &ruta, RedSucursales &red);

    // Carga conexiones.csv — requiere que las sucursales ya estén en la red
    int cargarConexiones(const std::string &ruta, RedSucursales &red);

    // Carga productos_fase2.csv — distribuye cada producto a su sucursal
    int cargarProductos(const std::string &ruta, RedSucursales &red);

    // Carga los tres archivos en el orden correcto
    bool cargarTodo(const std::string &rutaSucursales,
                    const std::string &rutaConexiones,
                    const std::string &rutaProductos,
                    RedSucursales &red);
};

#endif // CARGADOR_RED_H
