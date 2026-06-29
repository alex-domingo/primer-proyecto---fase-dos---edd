#include "SimuladorTransferencia.h"
#include <QDateTime>

SimuladorTransferencia::SimuladorTransferencia(QObject *parent)
    : QObject(parent)
    , red(nullptr)
    , factor(30)
    , indiceTramo(0)
    , activo(false)
    , criterio(RedSucursales::TIEMPO)
{}

// ── Iniciar simulación de lote ────────────────────────────────

void SimuladorTransferencia::iniciar(RedSucursales *r,
                                     const std::vector<std::string> &codigosBarra,
                                     const std::string &origenId,
                                     const std::string &destinoId,
                                     RedSucursales::Criterio crit,
                                     int factorAceleracion) {
    if (activo) {
        emit log("[!] Ya hay una simulacion en curso.");
        return;
    }
    red      = r;
    criterio = crit;
    factor   = factorAceleracion > 0 ? factorAceleracion : 30;

    Sucursal *origen = red->buscarSucursal(origenId);
    if (!origen) { emit simulacionCompletada(false); return; }

    // Construir el lote con los productos solicitados
    lote.clear();
    for (const std::string &cod : codigosBarra) {
        Producto *p = origen->buscarPorCodigo(cod);
        if (p) {
            Producto copia = *p;
            copia.estado = "EnTransito";
            lote.push_back(copia);
        }
    }

    if (lote.empty()) {
        emit log("[!] Ningun producto valido en el lote.");
        emit simulacionCompletada(false);
        return;
    }

    ruta = red->rutaOptima(origenId, destinoId, criterio);
    if (!ruta.encontrada) {
        emit log("[!] No existe ruta entre las sucursales.");
        emit simulacionCompletada(false);
        return;
    }

    activo = true;
    indiceTramo = 0;

    emit log("=== Iniciando transferencia de lote ===");
    emit log(QString("Productos en lote: %1  |  Ruta: %2 nodos  |  Factor: %3x")
                 .arg(lote.size()).arg(ruta.nodos.size()).arg(factor));

    procesarNodo();
}

// ── Procesar el nodo actual de la ruta ────────────────────────

void SimuladorTransferencia::procesarNodo() {
    if (!activo) return;

    int n = (int)ruta.nodos.size();
    if (indiceTramo >= n) {
        terminarSimulacion(true);
        return;
    }

    QString sucId = QString::fromStdString(ruta.nodos[indiceTramo]);
    Sucursal *s = red->buscarSucursal(ruta.nodos[indiceTramo]);
    if (!s) { terminarSimulacion(false); return; }

    bool esOrigen  = (indiceTramo == 0);
    bool esDestino = (indiceTramo == n - 1);

    // ═══════════════════════════════════════════════════════════
    // Procesamiento UNO A UNO en cada cola (FIFO visible):
    // los productos entran a la cola y van saliendo del frente de
    // uno en uno, esperando el tiempo correspondiente entre cada uno.
    // Esto aplica igual a ingreso, traspaso y salida.
    //
    // Flujo por rol:
    //   ORIGEN     : ingreso(1x1) → salida(1x1) → viaja
    //   INTERMEDIA : ingreso(1x1) → traspaso(1x1) → salida(1x1) → viaja
    //   DESTINO    : ingreso(1x1) → PERMANECE
    // ═══════════════════════════════════════════════════════════

    // Encadenamiento de fases mediante callbacks.
    // Primero definimos qué hacer al terminar la cola de salida (viaje).
    auto viajarAlSiguiente = [=]() {
        if (!activo) return;
        std::string siguiente = ruta.nodos[indiceTramo + 1];
        int tiempoArista = 0;
        for (const Conexion &c : red->obtenerConexionesDe(ruta.nodos[indiceTramo])) {
            if (c.destinoId == siguiente) { tiempoArista = (int)c.tiempo; break; }
        }
        emit productoEnTransito(sucId, QString::fromStdString(siguiente));
        emit log(QString("  -> [%1 -> %2] LOTE EN VIAJE (%3s)")
                     .arg(sucId).arg(QString::fromStdString(siguiente)).arg(tiempoArista));
        QTimer::singleShot(msEscalado(tiempoArista), this, [=]() {
            if (!activo) return;
            indiceTramo++;
            procesarNodo();
        });
    };

    // Fase SALIDA: despacha uno a uno con el intervalo de despacho
    auto faseSalida = [=]() {
        if (!activo) return;
        for (const Producto &p : lote) s->getColaSalida().encolar(p);
        emit etapaIniciada(sucId, DESPACHO,
                           QString("[%1] %2 productos en cola de salida — despacho uno a uno")
                               .arg(sucId).arg(lote.size()));
        emit log(QString("  -> [%1] SALIDA: %2 productos, intervalo %3s c/u")
                     .arg(sucId).arg(lote.size()).arg(s->getTiempoDespacho()));

        int total = (int)lote.size();
        int intervalo = s->getTiempoDespacho();
        for (int i = 0; i < total; i++) {
            // i+1 para que el primero también espere su intervalo (entra, espera, sale)
            QTimer::singleShot(msEscalado(intervalo * (i + 1)), this, [=]() {
                if (!activo) return;
                if (!s->getColaSalida().estaVacia()) {
                    QString nombreProd =
                        QString::fromStdString(s->getColaSalida().frente().nombre);
                    s->getColaSalida().desencolar();
                    emit productoDespachado(i + 1, total, nombreProd);
                    emit log(QString("     SALIDA despachado [%1/%2]: %3")
                                 .arg(i + 1).arg(total).arg(nombreProd));
                }
                if (i == total - 1) viajarAlSiguiente();
            });
        }
    };

    // Fase TRASPASO: solo intermedias, procesa uno a uno
    auto faseTraspaso = [=]() {
        if (!activo) return;
        for (const Producto &p : lote) s->getColaTraspaso().encolar(p);
        emit etapaIniciada(sucId, TRASPASO,
                           QString("[%1] %2 productos en cola de traspaso — uno a uno")
                               .arg(sucId).arg(lote.size()));
        emit log(QString("  -> [%1] TRASPASO: %2 productos, %3s c/u")
                     .arg(sucId).arg(lote.size()).arg(s->getTiempoTraspaso()));

        int total = (int)lote.size();
        int tTras = s->getTiempoTraspaso();
        for (int i = 0; i < total; i++) {
            QTimer::singleShot(msEscalado(tTras * (i + 1)), this, [=]() {
                if (!activo) return;
                if (!s->getColaTraspaso().estaVacia()) {
                    QString nombreProd =
                        QString::fromStdString(s->getColaTraspaso().frente().nombre);
                    s->getColaTraspaso().desencolar();
                    emit productoDespachado(i + 1, total, nombreProd);
                    emit log(QString("     TRASPASO procesado [%1/%2]: %3")
                                 .arg(i + 1).arg(total).arg(nombreProd));
                }
                if (i == total - 1) faseSalida();
            });
        }
    };

    // ── Fase INGRESO: todas las sucursales, uno a uno ─────────
    for (const Producto &p : lote) s->getColaIngreso().encolar(p);
    emit etapaIniciada(sucId, INGRESO,
                       QString("[%1] %2 productos en cola de ingreso — uno a uno")
                           .arg(sucId).arg(lote.size()));
    emit log(QString("  -> [%1] INGRESO: %2 productos, %3s c/u%4")
                 .arg(sucId).arg(lote.size()).arg(s->getTiempoIngreso())
                 .arg(esDestino ? " [destino final]" :
                          esOrigen ? " [origen]" : " [intermedia]"));

    int total = (int)lote.size();
    int tIng = s->getTiempoIngreso();

    if (esDestino) {
        // DESTINO: procesa el ingreso uno a uno. Cada producto se recibe,
        // sale de la cola de ingreso y queda almacenado en el INVENTARIO
        // de la sucursal (vía aplicarTransferenciaFinal). El producto
        // "permanece" en la sucursal — en su inventario, no en la cola.
        // Vaciar la cola es necesario para no bloquear futuras operaciones.
        for (int i = 0; i < total; i++) {
            QTimer::singleShot(msEscalado(tIng * (i + 1)), this, [=]() {
                if (!activo) return;
                if (!s->getColaIngreso().estaVacia()) {
                    QString nombreProd =
                        QString::fromStdString(s->getColaIngreso().frente().nombre);
                    s->getColaIngreso().desencolar();
                    emit productoDespachado(i + 1, total, nombreProd);
                    emit log(QString("     INGRESO recibido y almacenado [%1/%2]: %3")
                                 .arg(i + 1).arg(total).arg(nombreProd));
                }
                if (i == total - 1) {
                    for (Producto &p : lote) p.estado = "Disponible";
                    emit etapaIniciada(sucId, ENTREGADO,
                                       QString("[%1] %2 productos entregados y almacenados")
                                           .arg(sucId).arg(lote.size()));
                    emit log(QString("  -> [%1] ENTREGADO — lote almacenado en inventario")
                                 .arg(sucId));
                    aplicarTransferenciaFinal();
                    terminarSimulacion(true);
                }
            });
        }
    } else {
        // ORIGEN o INTERMEDIA: ingreso uno a uno, luego continúa
        for (int i = 0; i < total; i++) {
            QTimer::singleShot(msEscalado(tIng * (i + 1)), this, [=]() {
                if (!activo) return;
                if (!s->getColaIngreso().estaVacia()) {
                    QString nombreProd =
                        QString::fromStdString(s->getColaIngreso().frente().nombre);
                    s->getColaIngreso().desencolar();
                    emit productoDespachado(i + 1, total, nombreProd);
                    emit log(QString("     INGRESO procesado [%1/%2]: %3")
                                 .arg(i + 1).arg(total).arg(nombreProd));
                }
                if (i == total - 1) {
                    if (esOrigen) faseSalida();   // origen: sin traspaso
                    else          faseTraspaso(); // intermedia
                }
            });
        }
    }
}

// ── Terminar ──────────────────────────────────────────────────

void SimuladorTransferencia::terminarSimulacion(bool exitosa) {
    activo = false;
    if (exitosa)
        emit log("=== Transferencia de lote completada ===");
    else
        emit log("=== Transferencia cancelada o fallida ===");
    emit simulacionCompletada(exitosa);
}

// ── Aplicar cambios reales al inventario ──────────────────────

void SimuladorTransferencia::aplicarTransferenciaFinal() {
    if (!red || ruta.nodos.size() < 2) return;

    std::string origenId  = ruta.nodos.front();
    std::string destinoId = ruta.nodos.back();
    Sucursal *origen  = red->buscarSucursal(origenId);
    Sucursal *destino = red->buscarSucursal(destinoId);
    if (!origen || !destino) return;

    // Procesar cada producto del lote
    for (const Producto &prod : lote) {
        Producto *pOrig = origen->buscarPorCodigo(prod.codigoBarra);
        if (!pOrig) continue;

        int unidades = prod.stock; // transferimos el stock que tenía
        int stockRestante = pOrig->stock - unidades;

        // Copia del producto transferido para registrar en las pilas
        Producto registro = prod;
        registro.stock = unidades;

        // Destino: sumar o crear
        Producto *pDest = destino->buscarPorCodigo(prod.codigoBarra);
        if (pDest) {
            destino->getCatalogo()->actualizarStock(
                prod.codigoBarra, pDest->stock + unidades);
        } else {
            Producto nuevo = prod;
            nuevo.estado = "Disponible";
            nuevo.sucursalId = destinoId;
            destino->getCatalogo()->agregarProducto(nuevo);
        }

        // Origen: restar o eliminar
        if (stockRestante <= 0) {
            origen->eliminarProducto(
                pOrig->nombre, pOrig->codigoBarra,
                pOrig->categoria, pOrig->fechaCaducidad);
        } else {
            origen->getCatalogo()->actualizarStock(
                prod.codigoBarra, stockRestante);
        }

        // ── Registrar en las pilas (LIFO) para que el historial refleje
        //    el movimiento y se pueda hacer rollback ──
        // En el ORIGEN: la salida del producto (TRANSFERIR hacia destino)
        origen->registrarOperacion(
            Operacion(Operacion::TRANSFERIR, registro, origenId, destinoId));
        // En el DESTINO: la llegada del producto (AGREGAR al inventario)
        registro.estado = "Disponible";
        registro.sucursalId = destinoId;
        destino->registrarOperacion(
            Operacion(Operacion::AGREGAR, registro, destinoId));
    }
}

void SimuladorTransferencia::cancelar() {
    if (!activo) return;
    activo = false;
    emit log("=== Simulacion cancelada por el usuario ===");
    emit simulacionCompletada(false);
}
