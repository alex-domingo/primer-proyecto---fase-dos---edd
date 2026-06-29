#ifndef COLA_H
#define COLA_H

/*
 * Cola.h
 * -------
 * Cola genérica FIFO implementada con lista enlazada.
 * Usada por cada sucursal para sus tres colas:
 *   - ColaIngreso    : productos recién recibidos
 *   - ColaTraspaso   : productos en preparación para envío
 *   - ColaSalida     : productos listos para despachar
 *
 * Complejidades:
 *   encolar   -> O(1)
 *   desencolar -> O(1)
 *   frente    -> O(1)
 */

template <typename T>
class Cola {
private:
    struct NodoCola {
        T        dato;
        NodoCola *siguiente;
        explicit NodoCola(const T &dato) : dato(dato), siguiente(nullptr) {}
    };

    NodoCola *cabeza;   // frente de la cola (se desencola aquí)
    NodoCola *cola;     // final de la cola  (se encola aquí)
    int       tamano;

public:
    Cola() : cabeza(nullptr), cola(nullptr), tamano(0) {}

    ~Cola() {
        while (!estaVacia()) desencolar();
    }

    // Agrega al final — O(1)
    void encolar(const T &dato) {
        NodoCola *nuevo = new NodoCola(dato);
        if (cola) cola->siguiente = nuevo;
        else      cabeza = nuevo;
        cola = nuevo;
        tamano++;
    }

    // Elimina del frente — O(1)
    bool desencolar() {
        if (estaVacia()) return false;
        NodoCola *aBorrar = cabeza;
        cabeza = cabeza->siguiente;
        if (!cabeza) cola = nullptr;
        delete aBorrar;
        tamano--;
        return true;
    }

    // Devuelve referencia al frente sin eliminarlo — O(1)
    T& frente() { return cabeza->dato; }
    const T& frente() const { return cabeza->dato; }

    bool estaVacia() const { return tamano == 0; }
    int  obtenerTamano() const { return tamano; }

    // Devuelve copia del contenido en orden FIFO (frente → final)
    // sin modificar la cola. Útil para visualización.
    void obtenerContenido(T *destino, int maxItems) const {
        NodoCola *actual = cabeza;
        int i = 0;
        while (actual != nullptr && i < maxItems) {
            destino[i++] = actual->dato;
            actual = actual->siguiente;
        }
    }
};

#endif // COLA_H
