#ifndef PROYECTO_EDD_CARGADORCSV_H
#define PROYECTO_EDD_CARGADORCSV_H

#include "Catalogo.h"
#include <string>

/*
 * CargadorCSV.h
 * --------------
 * Se encarga de leer el archivo CSV y cargar los productos
 * al catálogo, aplicando todas las validaciones requeridas.
 *
 * Validaciones que realiza:
 *   1. Que el archivo exista y sea legible
 *   2. Líneas malformadas (campos faltantes) → loggear y saltar
 *   3. Código de barra duplicado → loggear y omitir
 *   4. Precio y stock no numéricos → loggear y saltar
 *   5. Campos vacíos obligatorios → loggear y saltar
 *
 * Formato CSV esperado (con comillas):
 *   "Nombre","CodigoBarra","Categoria","FechaCaducidad","Marca","Precio","Stock"
 *
 * Los errores se guardan en data/errors.log
 */

class CargadorCSV {
private:
    std::string rutaLog; // ruta del archivo de errores

    // Escribe una línea de error en el archivo de log
    void loggearError(int numLinea, const std::string &motivo,
                      const std::string &lineaOriginal) const;

    // Quita las comillas que rodean un campo CSV
    std::string limpiarComillas(const std::string &campo) const;

    // Separa una línea CSV en sus campos respetando las comillas
    bool parsearLinea(const std::string &linea,
                      std::string campos[], int numCamposEsperados) const;

    // Valida que una cadena represente un número decimal válido
    bool esNumeroValido(const std::string &texto) const;

    // Valida que una cadena represente un entero válido
    bool esEnteroValido(const std::string &texto) const;

public:
    explicit CargadorCSV(const std::string &rutaLog = "data/errors.log");

    // Carga el CSV al catálogo y retorna la cantidad de productos cargados
    int cargar(const std::string &rutaCSV, Catalogo &catalogo);
};

#endif //PROYECTO_EDD_CARGADORCSV_H
