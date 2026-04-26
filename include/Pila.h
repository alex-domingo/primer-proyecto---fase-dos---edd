#ifndef PILA_H
#define PILA_H

/*
 * Pila.h
 * -------
 * Pila genérica LIFO implementada con lista enlazada.
 * Usos en Fase 2:
 *   - PilaOperaciones : historial de cambios por sucursal (rollback/deshacer)
 *   - PilaDevoluciones: productos devueltos pendientes de reprocesar
 *
 * Complejidades:
 *   apilar    -> O(1)
 *   desapilar -> O(1)
 *   cima      -> O(1)
 */

template <typename T>
class Pila {
private:
    struct NodoPila {
        T        dato;
        NodoPila *siguiente;
        explicit NodoPila(const T &dato) : dato(dato), siguiente(nullptr) {}
    };

    NodoPila *cima_;
    int       tamano;

public:
    Pila() : cima_(nullptr), tamano(0) {}

    ~Pila() {
        while (!estaVacia()) desapilar();
    }

    // Agrega en la cima — O(1)
    void apilar(const T &dato) {
        NodoPila *nuevo = new NodoPila(dato);
        nuevo->siguiente = cima_;
        cima_ = nuevo;
        tamano++;
    }

    // Elimina la cima — O(1)
    bool desapilar() {
        if (estaVacia()) return false;
        NodoPila *aBorrar = cima_;
        cima_ = cima_->siguiente;
        delete aBorrar;
        tamano--;
        return true;
    }

    // Devuelve referencia a la cima sin eliminarla — O(1)
    T& cima() { return cima_->dato; }
    const T& cima() const { return cima_->dato; }

    bool estaVacia() const { return tamano == 0; }
    int  obtenerTamano() const { return tamano; }
};

#endif // PILA_H
