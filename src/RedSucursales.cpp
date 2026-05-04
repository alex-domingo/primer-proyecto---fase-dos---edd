#include "RedSucursales.h"
#include <iostream>
#include <fstream>
#include <limits>
#include <queue>
#include <algorithm>

// ── Constructor / destructor ──────────────────────────────────

RedSucursales::RedSucursales() {}

RedSucursales::~RedSucursales() {
    for (NodoGrafo *n : nodos) delete n;
    nodos.clear();
}

// ── Auxiliar: índice por ID ───────────────────────────────────

int RedSucursales::indiceDe(const std::string &id) const {
    for (int i = 0; i < (int)nodos.size(); i++) {
        if (nodos[i]->sucursal->getId() == id) return i;
    }
    return -1;
}

// ── Gestión del grafo ─────────────────────────────────────────

bool RedSucursales::agregarSucursal(Sucursal *sucursal) {
    if (!sucursal) return false;
    if (indiceDe(sucursal->getId()) != -1) {
        std::cout << "[Red] Sucursal '" << sucursal->getId() << "' ya existe.\n";
        return false;
    }
    nodos.push_back(new NodoGrafo(sucursal));
    return true;
}

bool RedSucursales::eliminarSucursal(const std::string &id) {
    int idx = indiceDe(id);
    if (idx == -1) return false;

    // Eliminar todas las conexiones que involucren esta sucursal
    for (NodoGrafo *n : nodos) {
        auto &conns = n->conexiones;
        conns.erase(
            std::remove_if(conns.begin(), conns.end(),
                           [&id](const Conexion &c) {
                               return c.origenId == id || c.destinoId == id;
                           }),
            conns.end());
    }
    delete nodos[idx];
    nodos.erase(nodos.begin() + idx);
    return true;
}

bool RedSucursales::agregarConexion(const std::string &origenId,
                                    const std::string &destinoId,
                                    double tiempo, double costo,
                                    bool bidireccional) {
    int io = indiceDe(origenId);
    int id = indiceDe(destinoId);
    if (io == -1 || id == -1) {
        std::cout << "[Red] Sucursal origen o destino no encontrada.\n";
        return false;
    }

    Conexion c{origenId, destinoId, tiempo, costo, bidireccional};
    nodos[io]->conexiones.push_back(c);

    if (bidireccional) {
        Conexion cr{destinoId, origenId, tiempo, costo, true};
        nodos[id]->conexiones.push_back(cr);
    }
    return true;
}

bool RedSucursales::eliminarConexion(const std::string &origenId,
                                     const std::string &destinoId) {
    int io = indiceDe(origenId);
    if (io == -1) return false;

    auto &conns = nodos[io]->conexiones;
    auto it = std::remove_if(conns.begin(), conns.end(),
                             [&destinoId](const Conexion &c) { return c.destinoId == destinoId; });
    if (it == conns.end()) return false;
    conns.erase(it, conns.end());
    return true;
}

// ── Consultas ─────────────────────────────────────────────────

Sucursal* RedSucursales::buscarSucursal(const std::string &id) const {
    int idx = indiceDe(id);
    return idx != -1 ? nodos[idx]->sucursal : nullptr;
}

bool RedSucursales::existeConexion(const std::string &o,
                                   const std::string &d) const {
    int io = indiceDe(o);
    if (io == -1) return false;
    for (const Conexion &c : nodos[io]->conexiones)
        if (c.destinoId == d) return true;
    return false;
}

int RedSucursales::contarSucursales() const {
    return (int)nodos.size();
}

int RedSucursales::contarConexiones() const {
    int total = 0;
    for (const NodoGrafo *n : nodos)
        total += (int)n->conexiones.size();
    return total;
}

std::vector<Sucursal*> RedSucursales::obtenerSucursales() const {
    std::vector<Sucursal*> resultado;
    for (const NodoGrafo *n : nodos)
        resultado.push_back(n->sucursal);
    return resultado;
}

std::vector<Conexion> RedSucursales::obtenerConexiones() const {
    std::vector<Conexion> resultado;
    for (const NodoGrafo *n : nodos)
        for (const Conexion &c : n->conexiones)
            resultado.push_back(c);
    return resultado;
}

std::vector<Conexion> RedSucursales::obtenerConexionesDe(
    const std::string &id) const {
    int idx = indiceDe(id);
    if (idx == -1) return {};
    return nodos[idx]->conexiones;
}

// ── Dijkstra ──────────────────────────────────────────────────

ResultadoRuta RedSucursales::dijkstra(const std::string &origenId,
                                      const std::string &destinoId,
                                      Criterio criterio) const {
    ResultadoRuta resultado;
    resultado.encontrada = false;
    resultado.pesoTotal  = 0.0;

    int V = (int)nodos.size();
    if (V == 0) return resultado;

    int io = indiceDe(origenId);
    int id = indiceDe(destinoId);
    if (io == -1 || id == -1) return resultado;

    const double INF = std::numeric_limits<double>::infinity();

    std::vector<double>      dist(V, INF);
    std::vector<int>         prev(V, -1);
    std::vector<bool>        visitado(V, false);

    // Min-heap: (distancia, índice)
    using Par = std::pair<double, int>;
    std::priority_queue<Par, std::vector<Par>, std::greater<Par>> pq;

    dist[io] = 0.0;
    pq.push({0.0, io});

    while (!pq.empty()) {
        auto [d, u] = pq.top(); pq.pop();
        if (visitado[u]) continue;
        visitado[u] = true;
        if (u == id) break;

        for (const Conexion &c : nodos[u]->conexiones) {
            int v = indiceDe(c.destinoId);
            if (v == -1 || visitado[v]) continue;

            double peso = (criterio == TIEMPO) ? c.tiempo : c.costo;
            double nuevaDist = dist[u] + peso;

            if (nuevaDist < dist[v]) {
                dist[v] = nuevaDist;
                prev[v] = u;
                pq.push({nuevaDist, v});
            }
        }
    }

    if (dist[id] == INF) return resultado; // no hay ruta

    // Reconstruir camino
    resultado.encontrada = true;
    resultado.pesoTotal  = dist[id];

    std::vector<std::string> camino;
    for (int cur = id; cur != -1; cur = prev[cur])
        camino.push_back(nodos[cur]->sucursal->getId());
    std::reverse(camino.begin(), camino.end());
    resultado.nodos = camino;

    return resultado;
}

ResultadoRuta RedSucursales::rutaOptima(const std::string &origenId,
                                        const std::string &destinoId,
                                        Criterio criterio) const {
    return dijkstra(origenId, destinoId, criterio);
}

double RedSucursales::calcularETA(const ResultadoRuta &ruta,
                                  Criterio criterio) const {
    if (!ruta.encontrada || ruta.nodos.empty()) return -1.0;

    double eta = ruta.pesoTotal; // peso de las aristas

    // Sumar tiempos de procesamiento de cada sucursal intermedia
    for (int i = 0; i < (int)ruta.nodos.size(); i++) {
        Sucursal *s = buscarSucursal(ruta.nodos[i]);
        if (!s) continue;

        bool esOrigen  = (i == 0);
        bool esDestino = (i == (int)ruta.nodos.size() - 1);

        if (criterio == TIEMPO) {
            if (!esOrigen && esDestino)
                eta += s->getTiempoIngreso();
            else if (!esOrigen && !esDestino)
                eta += s->getTiempoTraspaso() + s->getTiempoDespacho();
        }
    }
    return eta;
}

// ── Transferencia ─────────────────────────────────────────────

bool RedSucursales::transferirProducto(const std::string &codigoBarra,
                                       const std::string &origenId,
                                       const std::string &destinoId,
                                       Criterio criterio,
                                       int unidades) {
    Sucursal *origen  = buscarSucursal(origenId);
    Sucursal *destino = buscarSucursal(destinoId);
    if (!origen || !destino) {
        std::cout << "[Red] Sucursal origen o destino no encontrada.\n";
        return false;
    }

    Producto *p = origen->buscarPorCodigo(codigoBarra);
    if (!p) {
        std::cout << "[Red] Producto '" << codigoBarra
                  << "' no encontrado en " << origenId << ".\n";
        return false;
    }

    // Validar stock disponible
    if (unidades <= 0) unidades = p->stock; // 0 = transferir todo
    if (unidades > p->stock) {
        std::cout << "[Red] Stock insuficiente. Disponible: " << p->stock
                  << ", solicitado: " << unidades << ".\n";
        return false;
    }

    ResultadoRuta ruta = rutaOptima(origenId, destinoId, criterio);
    if (!ruta.encontrada) {
        std::cout << "[Red] No existe ruta de " << origenId
                  << " a " << destinoId << ".\n";
        return false;
    }

    // Copia del producto con el stock a transferir
    Producto copia = *p;
    copia.stock     = unidades;
    copia.estado    = "EnTransito";
    copia.sucursalId = destinoId;

    // Registrar operacion deshacible en origen
    origen->registrarOperacion(
        Operacion(Operacion::TRANSFERIR, copia, origenId, destinoId));

    // Simular flujo por sucursales intermedias
    for (int i = 1; i < (int)ruta.nodos.size(); i++) {
        Sucursal *s = buscarSucursal(ruta.nodos[i]);
        if (!s) continue;

        bool esDestino = (i == (int)ruta.nodos.size() - 1);
        if (esDestino) {
            copia.estado     = "Disponible";
            copia.sucursalId = destinoId;
            s->recibirProducto(copia);

            // Si el producto ya existe en destino: sumar stock en las 6 estructuras
            Producto *enDestino = s->buscarPorCodigo(codigoBarra);
            if (enDestino) {
                int nuevoStock = enDestino->stock + unidades;
                s->getCatalogo()->actualizarStock(codigoBarra, nuevoStock);
            } else {
                // No existe en destino: agregarlo como nuevo
                s->getCatalogo()->agregarProducto(copia);
            }
        } else {
            s->prepararTraspaso(copia);
            s->moverATraspasoASalida();
            s->despacharSiguiente();
        }
    }

    // En origen: restar unidades o eliminar si queda en 0
    int stockRestante = p->stock - unidades;
    if (stockRestante <= 0) {
        origen->eliminarProducto(
            copia.nombre, copia.codigoBarra,
            copia.categoria, copia.fechaCaducidad);
    } else {
        // Actualizar stock en las 6 estructuras del catálogo del origen
        origen->getCatalogo()->actualizarStock(copia.codigoBarra, stockRestante);
    }

    std::cout << "[Red] " << unidades << " unidades de '" << copia.nombre
              << "' transferidas de " << origenId << " a " << destinoId << ".\n";
    double eta = calcularETA(ruta, criterio);
    std::cout << "[Red] ETA estimado: " << eta << " segundos.\n";
    return true;
}

// ── Visualización ─────────────────────────────────────────────

void RedSucursales::mostrarRed() const {
    std::cout << "=== Red de Sucursales (" << nodos.size() << " nodos) ===\n";
    for (const NodoGrafo *n : nodos) {
        std::cout << "  [" << n->sucursal->getId() << "] "
                  << n->sucursal->getNombre()
                  << " — " << n->sucursal->getUbicacion()
                  << " (" << n->sucursal->contarProductos() << " productos)\n";
        for (const Conexion &c : n->conexiones) {
            std::cout << "      → " << c.destinoId
                      << "  tiempo=" << c.tiempo << "s"
                      << "  costo=Q" << c.costo << "\n";
        }
    }
}

bool RedSucursales::generarDot(const std::string &rutaArchivo) const {
    std::ofstream out(rutaArchivo);
    if (!out.is_open()) return false;

    out << "// Grafo de Red de Sucursales\n";
    out << "// dot -Tpng " << rutaArchivo << " -o grafo.png\n";
    out << "digraph RedSucursales {\n";
    out << "    graph [label=\"Red de Sucursales\" fontsize=14 rankdir=LR];\n";
    out << "    node  [shape=box style=filled fillcolor=lightblue"
           " fontname=Arial fontsize=11];\n";
    out << "    edge  [fontsize=9 fontname=Arial];\n\n";

    for (const NodoGrafo *n : nodos) {
        out << "    \"" << n->sucursal->getId() << "\""
            << " [label=\"" << n->sucursal->getId()
            << "\\n" << n->sucursal->getNombre()
            << "\\n" << n->sucursal->getUbicacion()
            << "\\n" << n->sucursal->contarProductos() << " productos\"];\n";
    }
    out << "\n";

    for (const NodoGrafo *n : nodos) {
        for (const Conexion &c : n->conexiones) {
            out << "    \"" << c.origenId << "\" -> \"" << c.destinoId << "\""
                << " [label=\"t=" << c.tiempo << "s\\nc=Q" << c.costo << "\""
                << (c.bidireccional ? " dir=both" : "") << "];\n";
        }
    }

    out << "}\n";
    out.close();
    std::cout << "[Dot] Grafo generado: " << rutaArchivo << "\n";
    return true;
}

bool RedSucursales::generarDotConRuta(const std::string &rutaArchivo,
                                      const ResultadoRuta &ruta) const {
    std::ofstream out(rutaArchivo);
    if (!out.is_open()) return false;

    out << "digraph RedSucursalesRuta {\n";
    out << "    graph [label=\"Ruta óptima\" fontsize=14 rankdir=LR];\n";
    out << "    node  [shape=box style=filled fillcolor=lightblue"
           " fontname=Arial fontsize=11];\n";
    out << "    edge  [fontsize=9 fontname=Arial];\n\n";

    // Nodos — resaltamos los de la ruta en amarillo
    for (const NodoGrafo *n : nodos) {
        bool enRuta = std::find(ruta.nodos.begin(), ruta.nodos.end(),
                                n->sucursal->getId()) != ruta.nodos.end();
        out << "    \"" << n->sucursal->getId() << "\""
            << " [label=\"" << n->sucursal->getId()
            << "\\n" << n->sucursal->getNombre() << "\""
            << (enRuta ? " fillcolor=gold" : "") << "];\n";
    }
    out << "\n";

    // Aristas — resaltamos las de la ruta en rojo
    for (const NodoGrafo *n : nodos) {
        for (const Conexion &c : n->conexiones) {
            bool enRuta = false;
            for (int i = 0; i + 1 < (int)ruta.nodos.size(); i++) {
                if (ruta.nodos[i] == c.origenId &&
                    ruta.nodos[i+1] == c.destinoId) {
                    enRuta = true; break;
                }
            }
            out << "    \"" << c.origenId << "\" -> \"" << c.destinoId << "\""
                << " [label=\"t=" << c.tiempo << "s\\nc=Q" << c.costo << "\""
                << (enRuta ? " color=red penwidth=2.5" : " color=gray")
                << "];\n";
        }
    }
    out << "}\n";
    out.close();
    return true;
}
