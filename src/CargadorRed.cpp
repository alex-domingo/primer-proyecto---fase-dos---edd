#include <cctype>
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

        // Formato flexible: 4 columnas (enunciado, todas bidireccionales)
        //                   o 5 columnas (con campo Bidireccional explícito).
        int numCampos = 1;
        {
            bool dentroComilla = false;
            for (char ch : linea) {
                if (ch == '"') dentroComilla = !dentroComilla;
                else if (ch == ',' && !dentroComilla) numCampos++;
            }
        }

        std::string c[5];
        std::string origen, destino, tStr, costoStr;
        bool bidireccional = true; // default: bidireccional (formato 4 cols)

        if (numCampos == 5) {
            if (!parsearLinea(linea, c, 5)) {
                loggearError(numLinea, "Linea malformada (5 campos)", linea);
                omitidas++; continue;
            }
            origen = c[0]; destino = c[1]; tStr = c[2]; costoStr = c[3];
            bidireccional = (c[4] == "true" || c[4] == "1" || c[4] == "TRUE");
        } else if (numCampos == 4) {
            if (!parsearLinea(linea, c, 4)) {
                loggearError(numLinea, "Linea malformada (4 campos)", linea);
                omitidas++; continue;
            }
            origen = c[0]; destino = c[1]; tStr = c[2]; costoStr = c[3];
            // bidireccional queda true por default
        } else {
            loggearError(numLinea,
                         "Numero de campos invalido (esperado 4 o 5, hay " +
                             std::to_string(numCampos) + ")", linea);
            omitidas++; continue;
        }

        if (origen.empty() || destino.empty()) {
            loggearError(numLinea, "Origen o destino vacio", linea); omitidas++; continue;
        }
        if (!esNumeroValido(tStr) || !esNumeroValido(costoStr)) {
            loggearError(numLinea, "Tiempo o costo no numerico", linea); omitidas++; continue;
        }
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

    // Detectar si el CSV usa el formato nuevo (entrada/salida) mirando
    // el encabezado. Si contiene "Entrada" y "Salida", es formato de hilos.
    bool formatoEntradaSalida = false;
    {
        std::string encabezado = linea;
        // normalizar a minúsculas para comparar
        for (char &ch : encabezado) ch = (char)std::tolower((unsigned char)ch);
        if (encabezado.find("entrada") != std::string::npos &&
            encabezado.find("salida") != std::string::npos) {
            formatoEntradaSalida = true;
        }
    }

    while (std::getline(archivo, linea)) {
        numLinea++;
        if (linea.empty() || linea == "\r") continue;
        if (!linea.empty() && linea.back() == '\r') linea.pop_back();

        // Contar campos fuera de comillas
        int numCampos = 1;
        {
            bool dentroComilla = false;
            for (char ch : linea) {
                if (ch == '"') dentroComilla = !dentroComilla;
                else if (ch == ',' && !dentroComilla) numCampos++;
            }
        }

        std::string c[10];
        std::string sucEntrada, sucSalida;
        std::string nombre, codigo, categoria, fecha, marca, precioStr, stockStr;
        std::string estado = "Disponible";

        if (formatoEntradaSalida) {
            // ── Formato NUEVO (hilos): 9 columnas ─────────────────
            //   SucursalEntradaId, SucursalSalidaId, Nombre, CodigoBarra,
            //   Categoria, FechaCaducidad, Marca, Precio, Stock
            // (opcional 10ª columna Estado)
            if (numCampos != 9 && numCampos != 10) {
                loggearError(numLinea,
                             "Formato entrada/salida espera 9 o 10 campos, hay " +
                                 std::to_string(numCampos), linea);
                omitidos++; continue;
            }
            if (!parsearLinea(linea, c, numCampos)) {
                loggearError(numLinea, "Linea malformada", linea);
                omitidos++; continue;
            }
            sucEntrada = c[0]; sucSalida = c[1];
            nombre = c[2]; codigo = c[3]; categoria = c[4];
            fecha = c[5]; marca = c[6]; precioStr = c[7]; stockStr = c[8];
            if (numCampos == 10 && !c[9].empty()) estado = c[9];
        } else {
            // ── Formato VIEJO: 8 columnas (SucursalID) o 9 (con Estado) ──
            std::string sucId;
            if (numCampos == 9) {
                if (!parsearLinea(linea, c, 9)) {
                    loggearError(numLinea, "Linea malformada (9 campos)", linea);
                    omitidos++; continue;
                }
                sucId = c[0]; nombre = c[1]; codigo = c[2]; categoria = c[3];
                fecha = c[4]; marca = c[5]; precioStr = c[6]; stockStr = c[7];
                estado = c[8].empty() ? "Disponible" : c[8];
            } else if (numCampos == 8) {
                if (!parsearLinea(linea, c, 8)) {
                    loggearError(numLinea, "Linea malformada (8 campos)", linea);
                    omitidos++; continue;
                }
                sucId = c[0]; nombre = c[1]; codigo = c[2]; categoria = c[3];
                fecha = c[4]; marca = c[5]; precioStr = c[6]; stockStr = c[7];
            } else {
                loggearError(numLinea,
                             "Numero de campos invalido (esperado 8 o 9, hay " +
                                 std::to_string(numCampos) + ")", linea);
                omitidos++; continue;
            }
            // En formato viejo, entrada = salida = la sucursal indicada
            sucEntrada = sucId;
            sucSalida  = sucId;
        }

        // Validaciones comunes
        if (sucEntrada.empty() || nombre.empty() || codigo.empty() ||
            categoria.empty() || fecha.empty() || marca.empty()) {
            loggearError(numLinea, "Campo obligatorio vacio", linea); omitidos++; continue;
        }
        if (sucSalida.empty()) sucSalida = sucEntrada; // sin salida → se queda
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

        // El producto ingresa físicamente en la sucursal de ENTRADA
        Sucursal *suc = red.buscarSucursal(sucEntrada);
        if (!suc) {
            loggearError(numLinea, "Sucursal entrada '" + sucEntrada + "' no existe", linea);
            omitidos++; continue;
        }
        // Validar que la sucursal de salida también exista (si es distinta)
        if (sucSalida != sucEntrada && !red.buscarSucursal(sucSalida)) {
            loggearError(numLinea, "Sucursal salida '" + sucSalida + "' no existe", linea);
            omitidos++; continue;
        }

        // Construir el producto con entrada/salida
        Producto p(nombre, codigo, categoria, fecha,
                   marca, precio, stock, sucEntrada, sucSalida, estado);

        if (suc->agregarProducto(p)) {
            cargados++;
            // Si entrada != salida, registrar como transferencia pendiente
            if (p.requiereTransferencia()) {
                red.registrarTransferenciaPendiente(codigo, sucEntrada, sucSalida);
            }
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
