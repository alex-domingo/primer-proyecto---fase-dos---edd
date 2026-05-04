#include "SimuladorTransferencia.h"
#include <QDateTime>

SimuladorTransferencia::SimuladorTransferencia(QObject *parent)
    : QObject(parent)
    , red(nullptr)
    , unidades(0)
    , factor(30)
    , indiceTramo(0)
    , activo(false)
    , criterio(RedSucursales::TIEMPO)
{
    timer.setSingleShot(true);
}

QString SimuladorTransferencia::etapaTexto(Etapa e) const {
    switch (e) {
    case INGRESO:   return "Ingreso";
    case TRASPASO:  return "Traspaso";
    case DESPACHO:  return "Despacho";
    case EN_VIAJE:  return "En viaje";
    case ENTREGADO: return "Entregado";
    }
    return "?";
}

// ── Iniciar la simulación ─────────────────────────────────────

void SimuladorTransferencia::iniciar(RedSucursales *r,
                                     const std::string &codigoBarra,
                                     const std::string &origenId,
                                     const std::string &destinoId,
                                     RedSucursales::Criterio crit,
                                     int unidadesParam,
                                     int factorAceleracion) {
    if (activo) {
        emit log("[!] Ya hay una simulación en curso.");
        return;
    }
    red      = r;
    criterio = crit;
    unidades = unidadesParam;
    factor   = factorAceleracion > 0 ? factorAceleracion : 30;

    Sucursal *origen = red->buscarSucursal(origenId);
    if (!origen) { emit simulacionCompletada(false); return; }

    Producto *p = origen->buscarPorCodigo(codigoBarra);
    if (!p) { emit simulacionCompletada(false); return; }

    if (unidades <= 0) unidades = p->stock;
    if (unidades > p->stock) {
        emit log(QString("[!] Stock insuficiente: %1 disponibles, %2 solicitados.")
                     .arg(p->stock).arg(unidades));
        emit simulacionCompletada(false);
        return;
    }

    ruta = red->rutaOptima(origenId, destinoId, criterio);
    if (!ruta.encontrada) {
        emit log("[!] No existe ruta entre las sucursales.");
        emit simulacionCompletada(false);
        return;
    }

    // Copia del producto que viajará — marcado en tránsito
    producto = *p;
    producto.stock = unidades;
    producto.estado = "EnTransito";

    activo = true;
    indiceTramo = 0;

    emit log(QString("=== Iniciando transferencia ==="));
    emit log(QString("Producto: %1  |  Unidades: %2")
                 .arg(QString::fromStdString(producto.nombre)).arg(unidades));
    emit log(QString("Ruta: %1 nodos  |  Factor: %2x")
                 .arg(ruta.nodos.size()).arg(factor));

    // El producto entra a la cola de ingreso del origen primero
    procesarSiguienteEtapa();
}

// ── Máquina de estados ────────────────────────────────────────

void SimuladorTransferencia::procesarSiguienteEtapa() {
    if (!activo) return;

    int n = (int)ruta.nodos.size();
    if (indiceTramo >= n) {
        // Llegó al destino
        terminarSimulacion(true);
        return;
    }

    QString sucId = QString::fromStdString(ruta.nodos[indiceTramo]);
    Sucursal *s = red->buscarSucursal(ruta.nodos[indiceTramo]);
    if (!s) { terminarSimulacion(false); return; }

    bool esOrigen  = (indiceTramo == 0);
    bool esDestino = (indiceTramo == n - 1);

    // ── Etapa INGRESO ─────────────────────────────────────────
    s->getColaIngreso().encolar(producto);
    emit etapaIniciada(sucId, INGRESO,
                       QString("[%1] Producto en cola de ingreso (%2 unidades)")
                           .arg(sucId).arg(unidades));
    emit log(QString("  → [%1] INGRESO  (%2s)")
                 .arg(sucId).arg(s->getTiempoIngreso()));

    QTimer::singleShot(msEscalado(s->getTiempoIngreso()), this, [=]() {
        if (!activo) return;
        // Sale de cola ingreso
        if (!s->getColaIngreso().estaVacia())
            s->getColaIngreso().desencolar();

        if (esDestino) {
            // El destino solo procesa ingreso — luego entrega
            emit etapaIniciada(sucId, ENTREGADO,
                               QString("[%1] Producto entregado").arg(sucId));
            aplicarTransferenciaFinal();
            terminarSimulacion(true);
            return;
        }

        // ── Etapa TRASPASO ────────────────────────────────────
        s->getColaTraspaso().encolar(producto);
        emit etapaIniciada(sucId, TRASPASO,
                           QString("[%1] Producto en cola de traspaso").arg(sucId));
        emit log(QString("  → [%1] TRASPASO (%2s)")
                     .arg(sucId).arg(s->getTiempoTraspaso()));

        QTimer::singleShot(msEscalado(s->getTiempoTraspaso()), this, [=]() {
            if (!activo) return;
            if (!s->getColaTraspaso().estaVacia())
                s->getColaTraspaso().desencolar();

            // ── Etapa DESPACHO ────────────────────────────────
            s->getColaSalida().encolar(producto);
            emit etapaIniciada(sucId, DESPACHO,
                               QString("[%1] Producto en cola de salida").arg(sucId));
            emit log(QString("  → [%1] DESPACHO (%2s)")
                         .arg(sucId).arg(s->getTiempoDespacho()));

            QTimer::singleShot(msEscalado(s->getTiempoDespacho()), this, [=]() {
                if (!activo) return;
                if (!s->getColaSalida().estaVacia())
                    s->getColaSalida().desencolar();

                // ── Etapa EN_VIAJE ────────────────────────────
                std::string siguiente = ruta.nodos[indiceTramo + 1];
                int tiempoArista = 0;
                for (const Conexion &c : red->obtenerConexionesDe(ruta.nodos[indiceTramo])) {
                    if (c.destinoId == siguiente) {
                        tiempoArista = (int)c.tiempo;
                        break;
                    }
                }
                emit productoEnTransito(sucId, QString::fromStdString(siguiente));
                emit log(QString("  → [%1 → %2] EN VIAJE (%3s)")
                             .arg(sucId)
                             .arg(QString::fromStdString(siguiente))
                             .arg(tiempoArista));

                QTimer::singleShot(msEscalado(tiempoArista), this, [=]() {
                    if (!activo) return;
                    indiceTramo++;
                    procesarSiguienteEtapa();
                });
            });
        });
        (void)esOrigen; // suprimir warning si no se usa
    });
}

// ── Terminar y aplicar cambios al inventario ──────────────────

void SimuladorTransferencia::terminarSimulacion(bool exitosa) {
    activo = false;
    emit simulacionCompletada(exitosa);
    if (exitosa)
        emit log("=== Transferencia completada exitosamente ===");
    else
        emit log("=== Transferencia cancelada o fallida ===");
}

void SimuladorTransferencia::aplicarTransferenciaFinal() {
    if (!red || ruta.nodos.size() < 2) return;

    std::string origenId  = ruta.nodos.front();
    std::string destinoId = ruta.nodos.back();

    Sucursal *origen  = red->buscarSucursal(origenId);
    Sucursal *destino = red->buscarSucursal(destinoId);
    if (!origen || !destino) return;

    Producto *pOrig = origen->buscarPorCodigo(producto.codigoBarra);
    if (!pOrig) return;

    int stockRestante = pOrig->stock - unidades;

    // Aplicar al destino
    Producto *pDest = destino->buscarPorCodigo(producto.codigoBarra);
    if (pDest) {
        // Existe: sumar stock en las 6 estructuras
        destino->getCatalogo()->actualizarStock(
            producto.codigoBarra, pDest->stock + unidades);
    } else {
        // No existe: agregar copia con estado Disponible
        Producto nuevo = producto;
        nuevo.estado = "Disponible";
        nuevo.sucursalId = destinoId;
        destino->getCatalogo()->agregarProducto(nuevo);
    }

    // Aplicar al origen
    if (stockRestante <= 0) {
        origen->eliminarProducto(
            pOrig->nombre, pOrig->codigoBarra,
            pOrig->categoria, pOrig->fechaCaducidad);
    } else {
        origen->getCatalogo()->actualizarStock(
            producto.codigoBarra, stockRestante);
    }
}

void SimuladorTransferencia::cancelar() {
    if (!activo) return;
    activo = false;
    timer.stop();
    emit log("=== Simulación cancelada por el usuario ===");
    emit simulacionCompletada(false);
}
