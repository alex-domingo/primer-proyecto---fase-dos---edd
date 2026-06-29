#ifndef SIMULADOR_TRANSFERENCIA_H
#define SIMULADOR_TRANSFERENCIA_H

#include <QObject>
#include <QTimer>
#include <QString>
#include <vector>
#include "RedSucursales.h"
#include "Producto.h"

/*
 * SimuladorTransferencia.h — Fase 2 (lotes)
 * ------------------------------------------
 * Simula el traslado de UNO O VARIOS productos a través de la red,
 * respetando los tres tiempos de cada sucursal:
 *
 *   - Tiempo de ingreso  : por producto, al llegar (siempre)
 *   - Tiempo de traspaso : por producto, solo en sucursales intermedias
 *   - Intervalo de despacho : ESPACIADO entre cada producto que sale
 *                             de la cola de salida (clave del FIFO)
 *
 * Flujo de colas según rol (enunciado):
 *   ORIGEN     : ingreso → salida (espaciada por intervalo) → viaja
 *   INTERMEDIA : ingreso → traspaso → salida (espaciada) → viaja
 *   DESTINO    : ingreso → PERMANECE
 *
 * Con un lote de N productos, la cola de salida se llena con los N
 * y el intervalo de despacho los suelta uno a uno en orden FIFO,
 * haciendo visible el funcionamiento de la cola.
 */
class SimuladorTransferencia : public QObject {
    Q_OBJECT

public:
    enum Etapa { INGRESO, TRASPASO, DESPACHO, EN_VIAJE, ENTREGADO };

    explicit SimuladorTransferencia(QObject *parent = nullptr);

    // Inicia la simulación de un lote de productos (por códigos de barra)
    void iniciar(RedSucursales *red,
                 const std::vector<std::string> &codigosBarra,
                 const std::string &origenId,
                 const std::string &destinoId,
                 RedSucursales::Criterio criterio,
                 int factorAceleracion = 30);

    void cancelar();
    bool estaActivo() const { return activo; }

signals:
    void etapaIniciada(const QString &sucursalId, int etapa,
                       const QString &mensaje);
    void productoEnTransito(const QString &origenId, const QString &destinoId);
    void productoDespachado(int indice, int total, const QString &nombreProducto);
    void simulacionCompletada(bool exitosa);
    void log(const QString &mensaje);

private:
    RedSucursales         *red;
    std::vector<Producto>  lote;        // productos a transferir
    ResultadoRuta          ruta;
    int                    factor;
    int                    indiceTramo;  // nodo actual en la ruta
    bool                   activo;
    RedSucursales::Criterio criterio;

    void procesarNodo();           // procesa el nodo actual de la ruta
    void terminarSimulacion(bool exitosa);
    void aplicarTransferenciaFinal();

    int msEscalado(int segundos) const { return (segundos * 1000) / factor; }
};

#endif // SIMULADOR_TRANSFERENCIA_H
