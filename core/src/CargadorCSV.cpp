#include "CargadorCSV.h"
#include <iostream>
#include <fstream>
#include <sstream>

/*
 * CargadorCSV.cpp
 * ----------------
 * Implementación del cargador de productos desde CSV.
 *
 * El parsing es la parte más delicada: los campos vienen
 * entre comillas y pueden contener comas dentro de ellas,
 * así que no podemos simplemente separar por comas.
 *
 * Estrategia de parseo:
 *   - Recorremos la línea caracter por caracter
 *   - Si encontramos '"' abrimos un campo entrecomillado
 *   - Dentro del campo, acumulamos hasta encontrar el '"' de cierre
 *   - Las comas fuera de comillas son separadores de campos
 */

CargadorCSV::CargadorCSV(const std::string &rutaLog) : rutaLog(rutaLog) {
}

// Guarda el error en errors.log con número de línea y motivo
void CargadorCSV::loggearError(int numLinea, const std::string &motivo,
                               const std::string &lineaOriginal) const {
    std::ofstream log(rutaLog, std::ios::app); // append — no sobreescribimos
    if (log.is_open()) {
        log << "[Linea " << numLinea << "] " << motivo
                << " | Contenido: " << lineaOriginal << "\n";
    }
}

// Quita las comillas que rodean un campo: "Leche Entera" → Leche Entera
std::string CargadorCSV::limpiarComillas(const std::string &campo) const {
    if (campo.size() >= 2 && campo.front() == '"' && campo.back() == '"') {
        return campo.substr(1, campo.size() - 2);
    }
    return campo;
}

/*
 * Parsea una línea CSV respetando comillas.
 * Retorna true si encontró exactamente numCamposEsperados campos.
 * Los campos se depositan en el arreglo campos[].
 */
bool CargadorCSV::parsearLinea(const std::string &linea,
                               std::string campos[],
                               int numCamposEsperados) const {
    int indiceCampo = 0;
    bool dentroComilla = false;
    std::string campoActual;

    for (size_t i = 0; i < linea.size(); i++) {
        char c = linea[i];

        if (c == '"') {
            dentroComilla = !dentroComilla;
            campoActual += c;
        } else if (c == ',' && !dentroComilla) {
            // Separador de campo — guardamos el campo actual
            if (indiceCampo < numCamposEsperados) {
                campos[indiceCampo] = limpiarComillas(campoActual);
            }
            indiceCampo++;
            campoActual.clear();
        } else {
            campoActual += c;
        }
    }

    // Último campo (no termina en coma)
    if (indiceCampo < numCamposEsperados) {
        campos[indiceCampo] = limpiarComillas(campoActual);
    }
    indiceCampo++;

    return indiceCampo == numCamposEsperados;
}

// Verifica que el string sea un número decimal (ej: "12.50", "8", "0.99")
bool CargadorCSV::esNumeroValido(const std::string &texto) const {
    if (texto.empty()) return false;
    bool tienePunto = false;
    for (size_t i = 0; i < texto.size(); i++) {
        char c = texto[i];
        if (i == 0 && c == '-') continue; // permite negativos (para detectarlos)
        if (c == '.' && !tienePunto) {
            tienePunto = true;
            continue;
        }
        if (c < '0' || c > '9') return false;
    }
    return true;
}

// Verifica que el string sea un entero no negativo (ej: "100", "0")
bool CargadorCSV::esEnteroValido(const std::string &texto) const {
    if (texto.empty()) return false;
    for (char c: texto) {
        if (c < '0' || c > '9') return false;
    }
    return true;
}

// ============================================================
// Método principal de carga
// ============================================================

int CargadorCSV::cargar(const std::string &rutaCSV, Catalogo &catalogo) {
    // Limpiamos el log anterior antes de empezar
    std::ofstream limpiarLog(rutaLog, std::ios::trunc);
    limpiarLog.close();

    // Verificamos que el archivo exista y sea legible
    std::ifstream archivo(rutaCSV);
    if (!archivo.is_open()) {
        std::cout << "[CargadorCSV] ERROR: no se pudo abrir '" << rutaCSV << "'.\n";
        std::cout << "              Verifica que el archivo exista en esa ruta.\n";
        return 0;
    }

    std::cout << "[CargadorCSV] Leyendo '" << rutaCSV << "'...\n";

    int cargados = 0;
    int omitidos = 0;
    int numLinea = 0;
    std::string linea;

    // Saltamos la primera línea (encabezado)
    std::getline(archivo, linea);
    numLinea++;

    const int NUM_CAMPOS = 7;

    while (std::getline(archivo, linea)) {
        numLinea++;

        // Ignoramos líneas completamente vacías
        if (linea.empty() || linea == "\r") continue;

        // Quitamos el \r si el archivo viene con saltos de línea Windows
        if (!linea.empty() && linea.back() == '\r') {
            linea.pop_back();
        }

        std::string campos[NUM_CAMPOS];

        // Validación 1: línea malformada (campos faltantes o de más)
        if (!parsearLinea(linea, campos, NUM_CAMPOS)) {
            loggearError(numLinea, "Linea malformada (campos incorrectos)", linea);
            omitidos++;
            continue;
        }

        // Los campos son: Nombre, CodigoBarra, Categoria,
        // FechaCaducidad, Marca, Precio y Stock
        std::string nombre = campos[0];
        std::string codigoBarra = campos[1];
        std::string categoria = campos[2];
        std::string fechaCaducidad = campos[3];
        std::string marca = campos[4];
        std::string precioStr = campos[5];
        std::string stockStr = campos[6];

        // Validación 2: campos obligatorios vacíos
        if (nombre.empty() || codigoBarra.empty() || categoria.empty() ||
            fechaCaducidad.empty() || marca.empty()) {
            loggearError(numLinea, "Campo obligatorio vacio", linea);
            omitidos++;
            continue;
        }

        // Validación 3: precio no numérico
        if (!esNumeroValido(precioStr)) {
            loggearError(numLinea, "Precio no es un numero valido: " + precioStr, linea);
            omitidos++;
            continue;
        }

        // Validación 4: stock no entero
        if (!esEnteroValido(stockStr)) {
            loggearError(numLinea, "Stock no es un entero valido: " + stockStr, linea);
            omitidos++;
            continue;
        }

        double precio = std::stod(precioStr);
        int stock = std::stoi(stockStr);

        // Validación 5: precio negativo
        if (precio < 0) {
            loggearError(numLinea, "Precio negativo: " + precioStr, linea);
            omitidos++;
            continue;
        }

        // Intentamos agregar al catálogo
        // agregarProducto() se encarga de verificar duplicados
        Producto p(nombre, codigoBarra, categoria, fechaCaducidad,
                   marca, precio, stock);

        if (catalogo.agregarProducto(p)) {
            cargados++;
        } else {
            // El catálogo rechazó el producto (duplicado u otro motivo)
            loggearError(numLinea, "Rechazado por el catalogo (posible duplicado)", linea);
            omitidos++;
        }
    }

    archivo.close();

    std::cout << "[CargadorCSV] Carga completada:\n";
    std::cout << "             Productos cargados : " << cargados << "\n";
    std::cout << "             Omitidos/con error : " << omitidos << "\n";
    if (omitidos > 0) {
        std::cout << "             Ver detalles en   : " << rutaLog << "\n";
    }

    return cargados;
}
