#include "ArbolB.h"
#include <iostream>

/*
 * ArbolB.cpp
 * -----------
 * Implementación del árbol B de orden t=3, clave: fecha de caducidad.
 *
 * La operación más importante de entender es la INSERCIÓN:
 * - Siempre insertamos en hojas
 * - Antes de bajar a un nodo, si está lleno (2t-1 claves) lo dividimos
 * - Al dividir, la clave del medio sube al padre
 * - Esto garantiza que cuando lleguemos a la hoja, haya espacio
 *
 * Para la ELIMINACIÓN hay 3 casos:
 *   Caso 1: la clave está en una hoja → la eliminamos directamente
 *   Caso 2: la clave está en un nodo interno → la reemplazamos con
 *            el predecesor o sucesor in-order y eliminamos ese
 *   Caso 3: la clave no está en este nodo → bajamos al hijo correcto,
 *            pero primero aseguramos que ese hijo tenga al menos t claves
 *            (si no, redistribuimos o fusionamos)
 */

// ============================================================
// Constructor y destructor
// ============================================================

ArbolB::ArbolB() : raiz(nullptr), tamano(0) {
}

ArbolB::~ArbolB() {
    destruirArbol(raiz);
}

void ArbolB::destruirArbol(NodoB *nodo) {
    if (nodo == nullptr) return;

    // Si no es hoja, destruimos primero todos los hijos
    if (!nodo->esHoja) {
        for (int i = 0; i <= nodo->numClaves; i++) {
            destruirArbol(nodo->hijos[i]);
        }
    }
    delete nodo;
}

// ============================================================
// Inserción
// ============================================================

void ArbolB::insertar(const Producto &producto) {
    const std::string &clave = producto.fechaCaducidad;

    // Árbol vacío: creamos la raíz
    if (raiz == nullptr) {
        raiz = new NodoB();
        raiz->claves[0] = clave;
        raiz->productos[0] = producto;
        raiz->numClaves = 1;
        tamano++;
        return;
    }

    // Si la raíz está llena, hay que dividirla antes de insertar
    if (raiz->numClaves == MAX_CLAVES) {
        NodoB *nuevaRaiz = new NodoB();
        nuevaRaiz->esHoja = false;
        nuevaRaiz->hijos[0] = raiz;

        // Dividimos la raíz vieja y la clave del medio sube a nuevaRaiz
        dividirHijo(nuevaRaiz, 0);
        raiz = nuevaRaiz;
    }

    insertarNoLleno(raiz, clave, producto);
    tamano++;
}

/*
 * Divide el hijo hijos[indice] del nodo padre.
 * El hijo debe estar lleno (MAX_CLAVES claves).
 * Después de dividir:
 *   - La mitad izquierda queda en el hijo original
 *   - La clave del medio sube al padre
 *   - La mitad derecha queda en un nuevo nodo hijo
 */
void ArbolB::dividirHijo(NodoB *padre, int indice) {
    NodoB *hijoLleno = padre->hijos[indice];
    NodoB *nuevoHijo = new NodoB();

    nuevoHijo->esHoja = hijoLleno->esHoja;
    nuevoHijo->numClaves = ORDEN_B - 1; // t-1 claves van al nuevo hijo

    // Copiamos las últimas (t-1) claves al nuevo hijo
    for (int j = 0; j < ORDEN_B - 1; j++) {
        nuevoHijo->claves[j] = hijoLleno->claves[j + ORDEN_B];
        nuevoHijo->productos[j] = hijoLleno->productos[j + ORDEN_B];
    }

    // Si no es hoja, copiamos también los últimos t hijos
    if (!hijoLleno->esHoja) {
        for (int j = 0; j < ORDEN_B; j++) {
            nuevoHijo->hijos[j] = hijoLleno->hijos[j + ORDEN_B];
        }
    }

    // El hijo original se queda con (t-1) claves
    hijoLleno->numClaves = ORDEN_B - 1;

    // Hacemos espacio en el padre para el nuevo hijo y la clave que sube
    for (int j = padre->numClaves; j >= indice + 1; j--) {
        padre->hijos[j + 1] = padre->hijos[j];
    }
    padre->hijos[indice + 1] = nuevoHijo;

    for (int j = padre->numClaves - 1; j >= indice; j--) {
        padre->claves[j + 1] = padre->claves[j];
        padre->productos[j + 1] = padre->productos[j];
    }

    // La clave del medio del hijo sube al padre
    padre->claves[indice] = hijoLleno->claves[ORDEN_B - 1];
    padre->productos[indice] = hijoLleno->productos[ORDEN_B - 1];
    padre->numClaves++;
}

/*
 * Inserta en un nodo que sabemos que NO está lleno.
 * Si es hoja: insertamos directamente manteniendo el orden.
 * Si es interior: bajamos al hijo correcto (dividiéndolo si está lleno).
 */
void ArbolB::insertarNoLleno(NodoB *nodo, const std::string &clave,
                             const Producto &producto) {
    int i = nodo->numClaves - 1;

    if (nodo->esHoja) {
        // Desplazamos las claves mayores para hacer espacio
        while (i >= 0 && clave < nodo->claves[i]) {
            nodo->claves[i + 1] = nodo->claves[i];
            nodo->productos[i + 1] = nodo->productos[i];
            i--;
        }
        // Insertamos en la posición correcta
        nodo->claves[i + 1] = clave;
        nodo->productos[i + 1] = producto;
        nodo->numClaves++;
    } else {
        // Encontramos el hijo correcto donde bajar
        while (i >= 0 && clave < nodo->claves[i]) {
            i--;
        }
        i++; // índice del hijo donde debe ir la clave

        // Si ese hijo está lleno, lo dividimos antes de bajar
        if (nodo->hijos[i]->numClaves == MAX_CLAVES) {
            dividirHijo(nodo, i);
            // Después del split, decidimos si bajamos por el hijo izq o der
            if (clave > nodo->claves[i]) {
                i++;
            }
        }
        insertarNoLleno(nodo->hijos[i], clave, producto);
    }
}

// ============================================================
// Búsqueda
// ============================================================

Producto *ArbolB::buscar(const std::string &fecha) const {
    return buscarEnNodo(raiz, fecha);
}

Producto *ArbolB::buscarEnNodo(NodoB *nodo, const std::string &clave) const {
    if (nodo == nullptr) return nullptr;

    // Buscamos la primera clave >= a la que buscamos
    int i = 0;
    while (i < nodo->numClaves && clave > nodo->claves[i]) {
        i++;
    }

    // ¿La encontramos exactamente?
    if (i < nodo->numClaves && clave == nodo->claves[i]) {
        return &nodo->productos[i];
    }

    // Si es hoja y no la encontramos, no existe
    if (nodo->esHoja) return nullptr;

    // Si no, bajamos al hijo correspondiente
    return buscarEnNodo(nodo->hijos[i], clave);
}

// ============================================================
// Búsqueda por rango de fechas
// ============================================================

void ArbolB::buscarRango(const std::string &fechaInicio,
                         const std::string &fechaFin) const {
    if (raiz == nullptr) {
        std::cout << "[Arbol B] El arbol esta vacio.\n";
        return;
    }

    std::cout << "Productos con caducidad entre "
            << fechaInicio << " y " << fechaFin << ":\n";
    std::cout << "--------------------------------------------\n";

    buscarRangoRec(raiz, fechaInicio, fechaFin);
}

/*
 * Recorre el árbol en orden. Para cada nodo:
 *  - Bajamos al hijo[i] antes de procesar claves[i]
 *  - Si claves[i] está en el rango, la mostramos
 *  - Al final bajamos al último hijo
 * Esto nos da las claves en orden cronológico.
 */
void ArbolB::buscarRangoRec(NodoB *nodo, const std::string &inicio,
                            const std::string &fin) const {
    if (nodo == nullptr) return;

    int i = 0;
    while (i < nodo->numClaves) {
        // Primero visitamos el subárbol izquierdo de claves[i]
        if (!nodo->esHoja && nodo->claves[i] >= inicio) {
            buscarRangoRec(nodo->hijos[i], inicio, fin);
        }

        // Luego revisamos si claves[i] está en el rango
        if (nodo->claves[i] >= inicio && nodo->claves[i] <= fin) {
            nodo->productos[i].mostrar();
        }

        // Si ya pasamos el fin, no tiene sentido seguir
        if (nodo->claves[i] > fin) break;

        i++;
    }

    // Visitamos el último subárbol derecho si aplica
    if (!nodo->esHoja && i == nodo->numClaves) {
        buscarRangoRec(nodo->hijos[i], inicio, fin);
    }
}

// ============================================================
// Listado en orden y utilidades
// ============================================================

void ArbolB::listarEnOrden() const {
    if (raiz == nullptr) {
        std::cout << "[Arbol B] El arbol esta vacio.\n";
        return;
    }
    listarRec(raiz);
}

// Recorrido in-order del árbol B: hijo[0], clave[0], hijo[1], clave[1]...
void ArbolB::listarRec(NodoB *nodo) const {
    if (nodo == nullptr) return;

    for (int i = 0; i < nodo->numClaves; i++) {
        if (!nodo->esHoja) {
            listarRec(nodo->hijos[i]);
        }
        nodo->productos[i].mostrar();
    }
    // Último hijo
    if (!nodo->esHoja) {
        listarRec(nodo->hijos[nodo->numClaves]);
    }
}

int ArbolB::obtenerTamano() const {
    return tamano;
}

bool ArbolB::estaVacio() const {
    return raiz == nullptr;
}

// ============================================================
// Eliminación
// ============================================================

bool ArbolB::eliminar(const std::string &fecha) {
    if (raiz == nullptr) return false;

    // Verificamos que exista antes de intentar eliminar
    if (buscar(fecha) == nullptr) return false;

    eliminarRec(raiz, fecha);
    tamano--;

    // Si la raíz quedó vacía tras una fusión, su único hijo es la nueva raíz
    if (raiz->numClaves == 0) {
        NodoB *viejaRaiz = raiz;
        raiz = (raiz->esHoja) ? nullptr : raiz->hijos[0];
        delete viejaRaiz;
    }

    return true;
}

/*
 * Eliminación recursiva — el nodo actual siempre tiene al menos t claves
 * (excepto la raíz), lo garantizamos ANTES de bajar a él.
 */
void ArbolB::eliminarRec(NodoB *nodo, const std::string &clave) {
    // Buscamos el índice de la primera clave >= clave buscada
    int idx = 0;
    while (idx < nodo->numClaves && nodo->claves[idx] < clave) {
        idx++;
    }

    bool claveEnEsteNodo = (idx < nodo->numClaves && nodo->claves[idx] == clave);

    if (claveEnEsteNodo) {
        if (nodo->esHoja) {
            eliminarDeHoja(nodo, idx); // Caso 1
        } else {
            eliminarDeInterior(nodo, idx); // Caso 2
        }
    } else {
        // La clave no está en este nodo — tenemos que bajar al hijo idx
        if (nodo->esHoja) return; // no debería pasar si verificamos antes

        // Caso 3: aseguramos que el hijo tenga suficientes claves antes de bajar
        bool esUltimoHijo = (idx == nodo->numClaves);
        if (nodo->hijos[idx]->numClaves < ORDEN_B) {
            rellenar(nodo, idx);
        }

        // Después de rellenar, el índice pudo cambiar si hubo fusión con previo
        if (esUltimoHijo && idx > nodo->numClaves) {
            eliminarRec(nodo->hijos[idx - 1], clave);
        } else {
            eliminarRec(nodo->hijos[idx], clave);
        }
    }
}

// Caso 1: la clave está en una hoja — simplemente la sacamos
void ArbolB::eliminarDeHoja(NodoB *nodo, int idx) {
    for (int i = idx + 1; i < nodo->numClaves; i++) {
        nodo->claves[i - 1] = nodo->claves[i];
        nodo->productos[i - 1] = nodo->productos[i];
    }
    nodo->numClaves--;
}

/*
 * Caso 2: la clave está en un nodo interno.
 * Subcaso 2a: el hijo izquierdo tiene >= t claves → usar predecesor
 * Subcaso 2b: el hijo derecho tiene >= t claves  → usar sucesor
 * Subcaso 2c: ambos tienen t-1 claves → fusionar y eliminar de ahí
 */
void ArbolB::eliminarDeInterior(NodoB *nodo, int idx) {
    std::string clave = nodo->claves[idx];

    if (nodo->hijos[idx]->numClaves >= ORDEN_B) {
        // 2a: reemplazamos con el predecesor
        std::string pred = obtenerPredecesor(nodo, idx);
        nodo->claves[idx] = pred;
        // Buscamos el producto del predecesor para actualizarlo también
        Producto *p = buscarEnNodo(nodo->hijos[idx], pred);
        if (p) nodo->productos[idx] = *p;
        eliminarRec(nodo->hijos[idx], pred);
    } else if (nodo->hijos[idx + 1]->numClaves >= ORDEN_B) {
        // 2b: reemplazamos con el sucesor
        std::string suc = obtenerSucesor(nodo, idx);
        nodo->claves[idx] = suc;
        Producto *p = buscarEnNodo(nodo->hijos[idx + 1], suc);
        if (p) nodo->productos[idx] = *p;
        eliminarRec(nodo->hijos[idx + 1], suc);
    } else {
        // 2c: fusionamos hijo izq + clave + hijo der, luego eliminamos
        fusionar(nodo, idx);
        eliminarRec(nodo->hijos[idx], clave);
    }
}

// Predecesor: la clave más a la derecha del subárbol izquierdo
std::string ArbolB::obtenerPredecesor(NodoB *nodo, int idx) {
    NodoB *actual = nodo->hijos[idx];
    while (!actual->esHoja) {
        actual = actual->hijos[actual->numClaves];
    }
    return actual->claves[actual->numClaves - 1];
}

// Sucesor: la clave más a la izquierda del subárbol derecho
std::string ArbolB::obtenerSucesor(NodoB *nodo, int idx) {
    NodoB *actual = nodo->hijos[idx + 1];
    while (!actual->esHoja) {
        actual = actual->hijos[0];
    }
    return actual->claves[0];
}

/*
 * rellenar: asegura que hijos[idx] tenga al menos t claves.
 * Primero intenta tomar prestada una clave del hermano izquierdo,
 * luego del derecho, y si ninguno tiene de sobra, fusiona.
 */
void ArbolB::rellenar(NodoB *nodo, int idx) {
    if (idx > 0 && nodo->hijos[idx - 1]->numClaves >= ORDEN_B) {
        tomarDePrevio(nodo, idx);
    } else if (idx < nodo->numClaves &&
               nodo->hijos[idx + 1]->numClaves >= ORDEN_B) {
        tomarDeSiguiente(nodo, idx);
    } else {
        // Ningún hermano tiene de sobra → fusionamos
        if (idx < nodo->numClaves) {
            fusionar(nodo, idx);
        } else {
            fusionar(nodo, idx - 1);
        }
    }
}

// Toma la última clave del hermano izquierdo y la rota por el padre
void ArbolB::tomarDePrevio(NodoB *nodo, int idx) {
    NodoB *hijo = nodo->hijos[idx];
    NodoB *previo = nodo->hijos[idx - 1];

    // Desplazamos todas las claves del hijo hacia la derecha
    for (int i = hijo->numClaves - 1; i >= 0; i--) {
        hijo->claves[i + 1] = hijo->claves[i];
        hijo->productos[i + 1] = hijo->productos[i];
    }
    if (!hijo->esHoja) {
        for (int i = hijo->numClaves; i >= 0; i--) {
            hijo->hijos[i + 1] = hijo->hijos[i];
        }
    }

    // La clave del padre baja al hijo
    hijo->claves[0] = nodo->claves[idx - 1];
    hijo->productos[0] = nodo->productos[idx - 1];
    if (!hijo->esHoja) {
        hijo->hijos[0] = previo->hijos[previo->numClaves];
    }

    // La última clave del previo sube al padre
    nodo->claves[idx - 1] = previo->claves[previo->numClaves - 1];
    nodo->productos[idx - 1] = previo->productos[previo->numClaves - 1];

    hijo->numClaves++;
    previo->numClaves--;
}

// Toma la primera clave del hermano derecho y la rota por el padre
void ArbolB::tomarDeSiguiente(NodoB *nodo, int idx) {
    NodoB *hijo = nodo->hijos[idx];
    NodoB *siguiente = nodo->hijos[idx + 1];

    // La clave del padre baja al final del hijo
    hijo->claves[hijo->numClaves] = nodo->claves[idx];
    hijo->productos[hijo->numClaves] = nodo->productos[idx];
    if (!hijo->esHoja) {
        hijo->hijos[hijo->numClaves + 1] = siguiente->hijos[0];
    }

    // La primera clave del siguiente sube al padre
    nodo->claves[idx] = siguiente->claves[0];
    nodo->productos[idx] = siguiente->productos[0];

    // Desplazamos las claves del siguiente hacia la izquierda
    for (int i = 1; i < siguiente->numClaves; i++) {
        siguiente->claves[i - 1] = siguiente->claves[i];
        siguiente->productos[i - 1] = siguiente->productos[i];
    }
    if (!siguiente->esHoja) {
        for (int i = 1; i <= siguiente->numClaves; i++) {
            siguiente->hijos[i - 1] = siguiente->hijos[i];
        }
    }

    hijo->numClaves++;
    siguiente->numClaves--;
}

/*
 * Fusiona hijos[idx] con hijos[idx+1], bajando claves[idx] del padre.
 * El resultado queda en hijos[idx] y hijos[idx+1] se elimina.
 */
void ArbolB::fusionar(NodoB *nodo, int idx) {
    NodoB *izq = nodo->hijos[idx];
    NodoB *der = nodo->hijos[idx + 1];

    // La clave del padre baja al medio del nodo fusionado
    izq->claves[ORDEN_B - 1] = nodo->claves[idx];
    izq->productos[ORDEN_B - 1] = nodo->productos[idx];

    // Copiamos las claves del hijo derecho al izquierdo
    for (int i = 0; i < der->numClaves; i++) {
        izq->claves[i + ORDEN_B] = der->claves[i];
        izq->productos[i + ORDEN_B] = der->productos[i];
    }
    if (!izq->esHoja) {
        for (int i = 0; i <= der->numClaves; i++) {
            izq->hijos[i + ORDEN_B] = der->hijos[i];
        }
    }
    izq->numClaves = 2 * ORDEN_B - 1;

    // Sacamos la clave del padre y ajustamos sus hijos
    for (int i = idx + 1; i < nodo->numClaves; i++) {
        nodo->claves[i - 1] = nodo->claves[i];
        nodo->productos[i - 1] = nodo->productos[i];
    }
    for (int i = idx + 2; i <= nodo->numClaves; i++) {
        nodo->hijos[i - 1] = nodo->hijos[i];
    }
    nodo->numClaves--;

    delete der; // ya copiamos todo, podemos liberar el hijo derecho
}
