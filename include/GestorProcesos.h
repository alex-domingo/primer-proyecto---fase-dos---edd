#ifndef GESTOR_PROCESOS_H
#define GESTOR_PROCESOS_H

#include <QObject>
#include <QTimer>
#include <QString>
#include <vector>
#include "RedSucursales.h"

/*
 * GestorProcesos.h — simulación visual concurrente ("hilos")
 * -----------------------------------------------------------
 * Maneja MÚLTIPLES transferencias a la vez para visualizar cómo
 * varios productos se mueven por la red simultáneamente.
 *
 * En lugar de timers encadenados por proceso (que serían difíciles
 * de coordinar entre varios), usa un MODELO DE TICK: un único timer
 * global avanza el "reloj" de la simulación, y en cada tick todos
 * los procesos activos avanzan según su tiempo acumulado.
 *
 * Esto simula concurrencia de forma determinista y visible: todos
 * los procesos comparten el mismo reloj pero progresan de forma
 * independiente según los tiempos de sus sucursales.
 *
 * Cada "proceso" representa un producto viajando entrada → salida.
 */
class GestorProcesos : public QObject {
    Q_OBJECT

public:
    enum EstadoProceso { ESPERANDO, INGRESO, TRASPASO, DESPACHO, VIAJE, COMPLETADO };

    struct Proceso {
        std::string   codigoBarra;
        std::string   nombreProducto;
        std::string   origenId;
        std::string   destinoId;
        ResultadoRuta ruta;
        int           tramoActual;     // índice del nodo actual en la ruta
        EstadoProceso estado;
        double        tiempoEnEtapa;   // segundos acumulados en la etapa actual
        double        tiempoEtapaTotal;// duración total de la etapa actual
        bool          activo;
    };

    explicit GestorProcesos(QObject *parent = nullptr);

    // Carga los procesos desde las transferencias pendientes de la red
    void prepararDesdePendientes(RedSucursales *red,
                                 RedSucursales::Criterio criterio);

    // Inicia la simulación concurrente de todos los procesos
    void iniciar(int factorAceleracion = 30);
    void cancelar();

    int  totalProcesos()      const { return (int)procesos.size(); }
    int  procesosCompletados() const;
    bool estaActivo()         const { return activo; }

    const std::vector<Proceso>& obtenerProcesos() const { return procesos; }

signals:
    void procesoActualizado(int indice);  // un proceso cambió de etapa
    void tick();                          // avanzó el reloj (refrescar UI)
    void todoCompletado();
    void log(const QString &mensaje);

private slots:
    void avanzarTick();

private:
    RedSucursales          *red;
    RedSucursales::Criterio criterio;
    std::vector<Proceso>    procesos;
    QTimer                  timer;
    int                     factor;
    bool                    activo;

    static constexpr int TICK_MS = 100; // resolución del reloj (100ms reales/tick)

    QString nombreEtapa(EstadoProceso e) const;
    void iniciarEtapa(Proceso &p, EstadoProceso etapa);
    double duracionEtapa(const Proceso &p, EstadoProceso etapa) const;
    void aplicarLlegada(Proceso &p);
};

#endif // GESTOR_PROCESOS_H
