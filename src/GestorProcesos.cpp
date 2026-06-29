#include "GestorProcesos.h"

GestorProcesos::GestorProcesos(QObject *parent)
    : QObject(parent)
    , red(nullptr)
    , criterio(RedSucursales::TIEMPO)
    , factor(30)
    , activo(false)
{
    timer.setInterval(TICK_MS);
    connect(&timer, &QTimer::timeout, this, &GestorProcesos::avanzarTick);
}

QString GestorProcesos::nombreEtapa(EstadoProceso e) const {
    switch (e) {
    case ESPERANDO:   return "Esperando";
    case INGRESO:     return "Ingreso";
    case TRASPASO:    return "Traspaso";
    case DESPACHO:    return "Despacho";
    case VIAJE:       return "En viaje";
    case COMPLETADO:  return "Completado";
    }
    return "?";
}

// ── Preparar procesos desde las pendientes ────────────────────

void GestorProcesos::prepararDesdePendientes(RedSucursales *r,
                                             RedSucursales::Criterio crit) {
    red = r;
    criterio = crit;
    procesos.clear();

    for (const auto &tp : red->obtenerPendientes()) {
        Proceso p;
        p.codigoBarra    = tp.codigoBarra;
        p.nombreProducto = tp.nombreProducto;
        p.origenId       = tp.origenId;
        p.destinoId      = tp.destinoId;
        p.ruta           = red->rutaOptima(tp.origenId, tp.destinoId, criterio);
        p.tramoActual    = 0;
        p.estado         = ESPERANDO;
        p.tiempoEnEtapa  = 0;
        p.tiempoEtapaTotal = 0;
        p.activo         = p.ruta.encontrada;
        procesos.push_back(p);
    }
}

// ── Iniciar simulación concurrente ────────────────────────────

void GestorProcesos::iniciar(int factorAceleracion) {
    if (activo || procesos.empty()) {
        if (procesos.empty()) emit log("[!] No hay procesos pendientes.");
        return;
    }
    factor = factorAceleracion > 0 ? factorAceleracion : 30;
    activo = true;

    emit log(QString("=== Iniciando %1 procesos concurrentes (factor %2x) ===")
                 .arg(procesos.size()).arg(factor));

    // Arrancar todos los procesos en la etapa de INGRESO de su origen
    for (int i = 0; i < (int)procesos.size(); i++) {
        if (procesos[i].activo) {
            iniciarEtapa(procesos[i], INGRESO);
            emit procesoActualizado(i);
        }
    }
    timer.start();
}

// ── Duración de cada etapa según la sucursal actual ───────────

double GestorProcesos::duracionEtapa(const Proceso &p, EstadoProceso etapa) const {
    if (p.tramoActual >= (int)p.ruta.nodos.size()) return 0;
    Sucursal *s = red->buscarSucursal(p.ruta.nodos[p.tramoActual]);
    if (!s) return 0;

    switch (etapa) {
    case INGRESO:  return s->getTiempoIngreso();
    case TRASPASO: return s->getTiempoTraspaso();
    case DESPACHO: return s->getTiempoDespacho();
    case VIAJE: {
        // tiempo de la arista al siguiente nodo
        if (p.tramoActual + 1 >= (int)p.ruta.nodos.size()) return 0;
        std::string sig = p.ruta.nodos[p.tramoActual + 1];
        for (const Conexion &c : red->obtenerConexionesDe(p.ruta.nodos[p.tramoActual]))
            if (c.destinoId == sig) return c.tiempo;
        return 0;
    }
    default: return 0;
    }
}

void GestorProcesos::iniciarEtapa(Proceso &p, EstadoProceso etapa) {
    p.estado = etapa;
    p.tiempoEnEtapa = 0;
    p.tiempoEtapaTotal = duracionEtapa(p, etapa);

    // Reflejar en las colas de la sucursal actual (para visualización)
    Sucursal *s = red->buscarSucursal(p.ruta.nodos[p.tramoActual]);
    if (!s) return;
    Producto *prod = s->buscarPorCodigo(p.codigoBarra);
    Producto copia = prod ? *prod : Producto();
    copia.estado = "EnTransito";

    switch (etapa) {
    case INGRESO:  s->getColaIngreso().encolar(copia);  break;
    case TRASPASO: s->getColaTraspaso().encolar(copia); break;
    case DESPACHO: s->getColaSalida().encolar(copia);   break;
    default: break;
    }
}

// ── Avance del reloj — el corazón de la concurrencia ──────────

void GestorProcesos::avanzarTick() {
    if (!activo) return;

    // Cuántos segundos simulados avanza cada tick
    double segPorTick = (TICK_MS / 1000.0) * factor;
    bool algunActivo = false;

    for (int i = 0; i < (int)procesos.size(); i++) {
        Proceso &p = procesos[i];
        if (!p.activo || p.estado == COMPLETADO) continue;
        algunActivo = true;

        p.tiempoEnEtapa += segPorTick;

        // ¿Terminó la etapa actual?
        if (p.tiempoEnEtapa >= p.tiempoEtapaTotal) {
            Sucursal *s = red->buscarSucursal(p.ruta.nodos[p.tramoActual]);
            int n = (int)p.ruta.nodos.size();
            bool esOrigen  = (p.tramoActual == 0);
            bool esDestino = (p.tramoActual == n - 1);

            // Desencolar de la cola de la etapa que termina
            auto desencolarDe = [&](Cola<Producto> &cola) {
                if (!cola.estaVacia()) cola.desencolar();
            };

            switch (p.estado) {
            case INGRESO:
                if (s) desencolarDe(s->getColaIngreso());
                if (esDestino) {
                    // Llegó al destino final: aplicar y completar
                    aplicarLlegada(p);
                    p.estado = COMPLETADO;
                    emit log(QString("  [%1] entregado en %2")
                                 .arg(QString::fromStdString(p.nombreProducto))
                                 .arg(QString::fromStdString(p.destinoId)));
                } else if (esOrigen) {
                    iniciarEtapa(p, DESPACHO); // origen: sin traspaso
                } else {
                    iniciarEtapa(p, TRASPASO); // intermedia
                }
                break;

            case TRASPASO:
                if (s) desencolarDe(s->getColaTraspaso());
                iniciarEtapa(p, DESPACHO);
                break;

            case DESPACHO:
                if (s) desencolarDe(s->getColaSalida());
                iniciarEtapa(p, VIAJE);
                break;

            case VIAJE:
                // Llegó al siguiente nodo
                p.tramoActual++;
                iniciarEtapa(p, INGRESO);
                break;

            default: break;
            }
            emit procesoActualizado(i);
        }
    }

    emit tick();

    if (!algunActivo) {
        activo = false;
        timer.stop();
        emit log("=== Todos los procesos completados ===");
        emit todoCompletado();
    }
}

// ── Aplicar la llegada al inventario ──────────────────────────

void GestorProcesos::aplicarLlegada(Proceso &p) {
    Sucursal *origen  = red->buscarSucursal(p.origenId);
    Sucursal *destino = red->buscarSucursal(p.destinoId);
    if (!origen || !destino) return;

    Producto *pOrig = origen->buscarPorCodigo(p.codigoBarra);
    if (!pOrig) return;

    int unidades = pOrig->stock; // se transfiere todo el stock

    // Sumar o crear en destino
    Producto *pDest = destino->buscarPorCodigo(p.codigoBarra);
    if (pDest) {
        destino->getCatalogo()->actualizarStock(
            p.codigoBarra, pDest->stock + unidades);
    } else {
        Producto nuevo = *pOrig;
        nuevo.estado = "Disponible";
        nuevo.sucursalId = p.destinoId;
        nuevo.sucursalEntradaId = p.destinoId;
        nuevo.sucursalSalidaId = p.destinoId;
        destino->getCatalogo()->agregarProducto(nuevo);
    }

    // Eliminar del origen (se transfiere todo)
    origen->eliminarProducto(
        pOrig->nombre, pOrig->codigoBarra,
        pOrig->categoria, pOrig->fechaCaducidad);
}

int GestorProcesos::procesosCompletados() const {
    int n = 0;
    for (const auto &p : procesos)
        if (p.estado == COMPLETADO) n++;
    return n;
}

void GestorProcesos::cancelar() {
    if (!activo) return;
    activo = false;
    timer.stop();
    emit log("=== Procesos cancelados ===");
    emit todoCompletado();
}
