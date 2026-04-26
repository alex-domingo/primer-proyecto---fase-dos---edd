#include "CargadorRed.h"
#include <iostream>
#include <fstream>

// ── Constructor ───────────────────────────────────────────────

CargadorRed::CargadorRed(const std::string &rutaLog) : rutaLog(rutaLog) {}

// ── Helpers (idénticos a CargadorCSV de Fase 1) ───────────────

void CargadorRed::loggearError(int linea, const std::string &motivo,
                                const std::string &lineaOriginal) const {
    std::ofstream log(rutaLog, std::ios::app);
    if (log.is_open())
        log << "[Linea " << linea << "] " << motivo
            << " | " << lineaOriginal << "\n";
}

std::string CargadorRed::limpiarComillas(const std::string &campo) const {
    if (campo.size() >= 2 && campo.front() == '"' && campo.back() == '"')
        return campo.substr(1, campo.size() - 2);
    return campo;
}

bool CargadorRed::parsearLinea(const std::string &linea,
                                std::string campos[],
                                int numEsperados) const {
    int    idx = 0;
    bool   dentroComilla = false;
    std::string actual;

    for (char c : linea) {
        if (c == '"') {
            dentroComilla = !dentroComilla;
            actual += c;
        } else if (c == ',' && !dentroComilla) {
            if (idx < numEsperados)
                campos[idx] = limpiarComillas(actual);
            idx++;
            actual.clear();
        } else {
            actual += c;
        }
    }
    if (idx < numEsperados)
        campos[idx] = limpiarComillas(actual);
    idx++;
    return idx == numEsperados;
}

bool CargadorRed::esEnteroValido(const std::string &t) const {
    if (t.empty()) return false;
    for (char c : t) if (c < '0' || c > '9') return false;
    return true;
}

bool CargadorRed::esNumeroValido(const std::string &t) const {
    if (t.empty()) return false;
    bool punto = false;
    for (size_t i = 0; i < t.size(); i++) {
        char c = t[i];
        if (i == 0 && c == '-') continue;
        if (c == '.' && !punto) { punto = true; continue; }
        if (c < '0' || c > '9') return false;
    }
    return true;
}

// ── 1. Cargar sucursales ──────────────────────────────────────

int CargadorRed::cargarSucursales(const std::string &ruta,
                                   RedSucursales &red) {
    std::ofstream(rutaLog, std::ios::trunc).close(); // limpiar log

    std::ifstream archivo(ruta);
    if (!archivo.is_open()) {
        std::cout << "[CargadorRed] No se pudo abrir '" << ruta << "'.\n";
        return 0;
    }

    std::cout << "[CargadorRed] Cargando sucursales desde '" << ruta << "'...\n";

    int cargadas = 0, omitidas = 0, numLinea = 0;
    std::string linea;
    std::getline(archivo, linea); // saltar encabezado
    numLinea++;

    while (std::getline(archivo, linea)) {
        numLinea++;
        if (linea.empty() || linea == "\r") continue;
        if (!linea.empty() && linea.back() == '\r') linea.pop_back();

        std::string c[6];
        if (!parsearLinea(linea, c, 6)) {
            loggearError(numLinea, "Linea malformada", linea); omitidas++; continue;
        }

        std::string id = c[0], nombre = c[1], ubicacion = c[2];
        std::string tI = c[3], tT = c[4], tD = c[5];

        if (id.empty() || nombre.empty() || ubicacion.empty()) {
            loggearError(numLinea, "Campo obligatorio vacio", linea); omitidas++; continue;
        }
        if (!esEnteroValido(tI) || !esEnteroValido(tT) || !esEnteroValido(tD)) {
            loggearError(numLinea, "Tiempo no entero", linea); omitidas++; continue;
        }

        Sucursal *s = new Sucursal(id, nombre, ubicacion,
                                   std::stoi(tI), std::stoi(tT), std::stoi(tD));
        if (red.agregarSucursal(s)) {
            cargadas++;
        } else {
            delete s; omitidas++;
            loggearError(numLinea, "Sucursal duplicada", linea);
        }
    }
    archivo.close();

    std::cout << "[CargadorRed] Sucursales cargadas: " << cargadas
              << "  omitidas: " << omitidas << "\n";
    return cargadas;
}

// ── 2. Cargar conexiones ──────────────────────────────────────

int CargadorRed::cargarConexiones(const std::string &ruta,
                                   RedSucursales &red) {
    std::ifstream archivo(ruta);
    if (!archivo.is_open()) {
        std::cout << "[CargadorRed] No se pudo abrir '" << ruta << "'.\n";
        return 0;
    }

    std::cout << "[CargadorRed] Cargando conexiones desde '" << ruta << "'...\n";

    int cargadas = 0, omitidas = 0, numLinea = 0;
    std::string linea;
    std::getline(archivo, linea); numLinea++;

    while (std::getline(archivo, linea)) {
        numLinea++;
        if (linea.empty() || linea == "\r") continue;
        if (!linea.empty() && linea.back() == '\r') linea.pop_back();

        std::string c[5];
        if (!parsearLinea(linea, c, 5)) {
            loggearError(numLinea, "Linea malformada", linea); omitidas++; continue;
        }

        std::string origen = c[0], destino = c[1];
        std::string tStr = c[2], costoStr = c[3], biStr = c[4];

        if (origen.empty() || destino.empty()) {
            loggearError(numLinea, "Origen o destino vacio", linea); omitidas++; continue;
        }
        if (!esNumeroValido(tStr) || !esNumeroValido(costoStr)) {
            loggearError(numLinea, "Tiempo o costo no numerico", linea); omitidas++; continue;
        }

        bool bidireccional = (biStr == "true" || biStr == "1");
        double tiempo = std::stod(tStr);
        double costo  = std::stod(costoStr);

        if (red.agregarConexion(origen, destino, tiempo, costo, bidireccional)) {
            cargadas++;
        } else {
            omitidas++;
            loggearError(numLinea, "Conexion no agregada (sucursal no existe?)", linea);
        }
    }
    archivo.close();

    std::cout << "[CargadorRed] Conexiones cargadas: " << cargadas
              << "  omitidas: " << omitidas << "\n";
    return cargadas;
}

// ── 3. Cargar productos ───────────────────────────────────────

int CargadorRed::cargarProductos(const std::string &ruta,
                                  RedSucursales &red) {
    std::ifstream archivo(ruta);
    if (!archivo.is_open()) {
        std::cout << "[CargadorRed] No se pudo abrir '" << ruta << "'.\n";
        return 0;
    }

    std::cout << "[CargadorRed] Cargando productos desde '" << ruta << "'...\n";

    int cargados = 0, omitidos = 0, numLinea = 0;
    std::string linea;
    std::getline(archivo, linea); numLinea++;

    while (std::getline(archivo, linea)) {
        numLinea++;
        if (linea.empty() || linea == "\r") continue;
        if (!linea.empty() && linea.back() == '\r') linea.pop_back();

        // Formato: SucursalID, Nombre, CodigoBarra, Categoria,
        //          FechaCaducidad, Marca, Precio, Stock, Estado
        std::string c[9];
        if (!parsearLinea(linea, c, 9)) {
            loggearError(numLinea, "Linea malformada", linea); omitidos++; continue;
        }

        std::string sucId    = c[0];
        std::string nombre   = c[1];
        std::string codigo   = c[2];
        std::string categoria= c[3];
        std::string fecha    = c[4];
        std::string marca    = c[5];
        std::string precioStr= c[6];
        std::string stockStr = c[7];
        std::string estado   = c[8];

        // Validaciones
        if (sucId.empty() || nombre.empty() || codigo.empty() ||
            categoria.empty() || fecha.empty() || marca.empty()) {
            loggearError(numLinea, "Campo obligatorio vacio", linea); omitidos++; continue;
        }
        if (codigo.size() != 10) {
            loggearError(numLinea, "Codigo barra no tiene 10 chars: " + codigo, linea);
            omitidos++; continue;
        }
        if (!esNumeroValido(precioStr)) {
            loggearError(numLinea, "Precio invalido: " + precioStr, linea); omitidos++; continue;
        }
        if (!esEnteroValido(stockStr)) {
            loggearError(numLinea, "Stock invalido: " + stockStr, linea); omitidos++; continue;
        }

        double precio = std::stod(precioStr);
        int    stock  = std::stoi(stockStr);
        if (precio < 0 || stock < 0) {
            loggearError(numLinea, "Precio o stock negativo", linea); omitidos++; continue;
        }
        if (estado.empty()) estado = "Disponible";

        // Buscar la sucursal destino
        Sucursal *suc = red.buscarSucursal(sucId);
        if (!suc) {
            loggearError(numLinea, "Sucursal '" + sucId + "' no existe", linea);
            omitidos++; continue;
        }

        Producto p(nombre, codigo, categoria, fecha,
                   marca, precio, stock, sucId, estado);

        if (suc->agregarProducto(p)) {
            cargados++;
        } else {
            loggearError(numLinea, "Rechazado por catalogo", linea);
            omitidos++;
        }
    }
    archivo.close();

    std::cout << "[CargadorRed] Productos cargados : " << cargados
              << "  omitidos: " << omitidos << "\n";
    return cargados;
}

// ── 4. Cargar todo ────────────────────────────────────────────

bool CargadorRed::cargarTodo(const std::string &rutaSucursales,
                              const std::string &rutaConexiones,
                              const std::string &rutaProductos,
                              RedSucursales &red) {
    std::cout << "\n[CargadorRed] === Carga completa de la red ===\n";

    int s = cargarSucursales(rutaSucursales, red);
    if (s == 0) {
        std::cout << "[CargadorRed] ERROR: sin sucursales, abortando.\n";
        return false;
    }

    int c = cargarConexiones(rutaConexiones, red);
    int p = cargarProductos(rutaProductos, red);

    std::cout << "[CargadorRed] === Resumen ===\n";
    std::cout << "  Sucursales : " << s << "\n";
    std::cout << "  Conexiones : " << c << "\n";
    std::cout << "  Productos  : " << p << "\n";
    std::cout << "  Errores    : ver " << rutaLog << "\n\n";

    return p > 0;
}
