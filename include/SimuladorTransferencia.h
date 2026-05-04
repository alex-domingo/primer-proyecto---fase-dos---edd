#ifndef SIMULADOR_TRANSFERENCIA_H
#define SIMULADOR_TRANSFERENCIA_H

#include <QObject>
#include <QTimer>
#include <QString>
#include "RedSucursales.h"
#include "Producto.h"

/*
 * SimuladorTransferencia.h
 * --------------------------
 * Simula el flujo de un producto a través de la red, respetando los
 * tiempos de cada sucursal (ingreso, traspaso, despacho) y los pesos
 * de las aristas, todo escalado por un factor de aceleración.
 *
 * Máquina de estados por sucursal en la ruta:
 *   1. INGRESO    — el producto entra a la cola de ingreso
 *   2. TRASPASO   — pasa a la cola de traspaso (preparación)
 *   3. DESPACHO   — pasa a la cola de salida (listo para enviar)
 *   4. EN_VIAJE   — viaja por la arista al siguiente nodo
 *
 * El nodo final no hace TRASPASO ni DESPACHO ni EN_VIAJE — solo INGRESO
 * y luego se considera entregado.
 *
 * Cada paso usa QTimer::singleShot con duración = (segundos * 1000) / factor.
 * Las señales permiten a la GUI actualizar visualmente.
 */
class SimuladorTransferencia : public QObject {
    Q_OBJECT

public:
    enum Etapa { INGRESO, TRASPASO, DESPACHO, EN_VIAJE, ENTREGADO };

    explicit SimuladorTransferencia(QObject *parent = nullptr);

    // Configura y arranca la simulación
    void iniciar(RedSucursales *red,
                 const std::string &codigoBarra,
                 const std::string &origenId,
                 const std::string &destinoId,
                 RedSucursales::Criterio criterio,
                 int unidades,
                 int factorAceleracion = 30); // 30x = 1 seg real ≈ 30s simulados

    void cancelar();
    bool estaActivo() const { return activo; }

signals:
    void etapaIniciada(const QString &sucursalId, int etapa,
                       const QString &mensaje);
    void productoEnTransito(const QString &origenId, const QString &destinoId);
    void simulacionCompletada(bool exitosa);
    void log(const QString &mensaje);

private:
    RedSucursales *red;
    Producto       producto;
    ResultadoRuta  ruta;
    int            unidades;
    int            factor;
    int            indiceTramo; // posición en ruta.nodos
    bool           activo;
    QTimer         timer;
    RedSucursales::Criterio criterio;

    // Avanza a la siguiente etapa
    void procesarSiguienteEtapa();
    void terminarSimulacion(bool exitosa);

    // Aplica los cambios reales a las estructuras (al final)
    void aplicarTransferenciaFinal();

    int  msEscalado(int segundos) const { return (segundos * 1000) / factor; }
    QString etapaTexto(Etapa e) const;
};

#endif // SIMULADOR_TRANSFERENCIA_H
