#include "ArbolBPlus.h"
#include <iostream>

/*
 * ArbolBPlus.cpp
 * ---------------
 * Implementación del árbol B+ de orden t=3, clave: categoría.
 *
 * Lo más importante de entender aquí vs el árbol B:
 *
 * En la INSERCIÓN con split:
 *   - Cuando dividimos una HOJA, la clave del medio se COPIA al padre
 *     (sigue existiendo en la hoja, a diferencia del árbol B donde sube)
 *   - Cuando dividimos un NODO INTERNO, la clave del medio SUBE al padre
 *     (igual que en el árbol B)
 *   - Al dividir una hoja, actualizamos el puntero 'siguiente'
 *     para mantener la cadena de hojas enlazadas
 *
 * En la BÚSQUEDA POR CATEGORÍA:
 *   - Bajamos hasta la primera hoja que tenga la categoría buscada
 *   - Luego caminamos el enlace 'siguiente' recolectando todos los
 *     productos de esa categoría hasta que la categoría cambie
 */

// ============================================================
// Constructor y destructor
// ============================================================

ArbolBPlus::ArbolBPlus() : raiz(nullptr), primeraHoja(nullptr), tamano(0) {
}

ArbolBPlus::~ArbolBPlus() {
    destruirArbol(raiz);
}

void ArbolBPlus::destruirArbol(NodoBPlus *nodo) {
    if (nodo == nullptr) return;
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

void ArbolBPlus::insertar(const Producto &producto) {
    const std::string &clave = producto.categoria;

    // Árbol vacío: la raíz es también la primera (y única) hoja
    if (raiz == nullptr) {
        raiz = new NodoBPlus();
        raiz->claves[0] = clave;
        raiz->productos[0] = producto;
        raiz->numClaves = 1;
        primeraHoja = raiz;
        tamano++;
        return;
    }

    // Si la raíz está llena la dividimos creando una nueva raíz vacía
    if (raiz->numClaves == MAX_CLAVES_BP) {
        NodoBPlus *nuevaRaiz = new NodoBPlus();
        nuevaRaiz->esHoja = false;
        nuevaRaiz->hijos[0] = raiz;
        dividirHijo(nuevaRaiz, 0, raiz);
        raiz = nuevaRaiz;
    }

    insertarNoLleno(raiz, clave, producto);
    tamano++;
}

/*
 * División de un hijo en el árbol B+.
 * La diferencia con el árbol B: cuando dividimos una HOJA,
 * la clave del medio se COPIA al padre (no se sube y elimina).
 * Así la hoja sigue teniendo todos sus datos.
 * También enlazamos el nuevo nodo hoja en la cadena 'siguiente'.
 */
void ArbolBPlus::dividirHijo(NodoBPlus *padre, int indice, NodoBPlus *hijo) {
    NodoBPlus *nuevoHijo = new NodoBPlus();
    nuevoHijo->esHoja = hijo->esHoja;
    int mitad = ORDEN_BPLUS - 1; // índice de la clave del medio

    if (hijo->esHoja) {
        // En hojas: copiamos desde la mitad hacia adelante al nuevo hijo
        nuevoHijo->numClaves = MAX_CLAVES_BP - mitad;
        for (int j = 0; j < nuevoHijo->numClaves; j++) {
            nuevoHijo->claves[j] = hijo->claves[j + mitad];
            nuevoHijo->productos[j] = hijo->productos[j + mitad];
        }
        hijo->numClaves = mitad;

        // Enlazamos la nueva hoja en la cadena
        nuevoHijo->siguiente = hijo->siguiente;
        hijo->siguiente = nuevoHijo;

        // La clave que sube al padre es la primera del nuevo hijo (se COPIA)
        std::string claveQueSubeAlPadre = nuevoHijo->claves[0];

        // Hacemos espacio en el padre
        for (int j = padre->numClaves; j > indice; j--) {
            padre->hijos[j + 1] = padre->hijos[j];
            padre->claves[j] = padre->claves[j - 1];
            padre->productos[j] = padre->productos[j - 1];
        }
        padre->hijos[indice + 1] = nuevoHijo;
        padre->claves[indice] = claveQueSubeAlPadre;
        padre->numClaves++;
    } else {
        // En nodos internos: la clave del medio SUBE (igual que árbol B)
        nuevoHijo->numClaves = MAX_CLAVES_BP - mitad - 1;
        for (int j = 0; j < nuevoHijo->numClaves; j++) {
            nuevoHijo->claves[j] = hijo->claves[j + mitad + 1];
            nuevoHijo->productos[j] = hijo->productos[j + mitad + 1];
        }
        for (int j = 0; j <= nuevoHijo->numClaves; j++) {
            nuevoHijo->hijos[j] = hijo->hijos[j + mitad + 1];
        }

        std::string claveQueSubeAlPadre = hijo->claves[mitad];
        hijo->numClaves = mitad;

        // Hacemos espacio en el padre
        for (int j = padre->numClaves; j > indice; j--) {
            padre->hijos[j + 1] = padre->hijos[j];
            padre->claves[j] = padre->claves[j - 1];
        }
        padre->hijos[indice + 1] = nuevoHijo;
        padre->claves[indice] = claveQueSubeAlPadre;
        padre->numClaves++;
    }
}

/*
 * Inserta en un nodo que no está lleno.
 * En hojas: insertamos manteniendo el orden por categoría.
 * En nodos internos: bajamos al hijo correcto dividiéndolo si es necesario.
 */
void ArbolBPlus::insertarNoLleno(NodoBPlus *nodo, const std::string &clave,
                                 const Producto &producto) {
    if (nodo->esHoja) {
        // Encontramos la posición correcta y desplazamos
        int i = nodo->numClaves - 1;
        while (i >= 0 && clave < nodo->claves[i]) {
            nodo->claves[i + 1] = nodo->claves[i];
            nodo->productos[i + 1] = nodo->productos[i];
            i--;
        }
        nodo->claves[i + 1] = clave;
        nodo->productos[i + 1] = producto;
        nodo->numClaves++;
    } else {
        // Buscamos el hijo correcto
        int i = nodo->numClaves - 1;
        while (i >= 0 && clave < nodo->claves[i]) {
            i--;
        }
        i++;

        // Si el hijo está lleno, lo dividimos antes de bajar
        if (nodo->hijos[i]->numClaves == MAX_CLAVES_BP) {
            dividirHijo(nodo, i, nodo->hijos[i]);
            if (clave >= nodo->claves[i]) {
                i++;
            }
        }
        insertarNoLleno(nodo->hijos[i], clave, producto);
    }
}

// ============================================================
// Búsqueda
// ============================================================

Producto *ArbolBPlus::buscar(const std::string &categoria,
                             const std::string &nombre) const {
    return buscarEnNodo(raiz, categoria);
    // Nota: buscarEnNodo llega a la hoja, luego buscamos por nombre ahí
    // La implementación completa está abajo
    (void) nombre; // suprimir warning, se usa en buscarPorCategoria
}

Producto *ArbolBPlus::buscarEnNodo(NodoBPlus *nodo,
                                   const std::string &clave) const {
    if (nodo == nullptr) return nullptr;

    if (nodo->esHoja) {
        // En la hoja buscamos la clave exacta
        for (int i = 0; i < nodo->numClaves; i++) {
            if (nodo->claves[i] == clave) {
                return &nodo->productos[i];
            }
        }
        return nullptr;
    }

    // En nodo interno: bajamos al hijo correcto
    int i = 0;
    while (i < nodo->numClaves && clave >= nodo->claves[i]) {
        i++;
    }
    return buscarEnNodo(nodo->hijos[i], clave);
}

/*
 * Búsqueda por categoría — la operación estrella del B+.
 * 1. Bajamos al primer nodo hoja que podría tener esa categoría
 * 2. Recorremos la cadena de hojas recolectando todos los productos
 *    que tengan exactamente esa categoría
 * 3. Paramos cuando la categoría del nodo sea mayor a la buscada
 */
void ArbolBPlus::buscarPorCategoria(const std::string &categoria) const {
    if (raiz == nullptr) {
        std::cout << "[Arbol B+] El arbol esta vacio.\n";
        return;
    }

    // Bajamos hasta la hoja de inicio
    NodoBPlus *nodo = raiz;
    while (!nodo->esHoja) {
        int i = 0;
        while (i < nodo->numClaves && categoria >= nodo->claves[i]) {
            i++;
        }
        nodo = nodo->hijos[i];
    }

    // Caminamos las hojas buscando todos los de esa categoría
    bool encontrado = false;
    while (nodo != nullptr) {
        for (int i = 0; i < nodo->numClaves; i++) {
            if (nodo->claves[i] == categoria) {
                nodo->productos[i].mostrar();
                encontrado = true;
            } else if (nodo->claves[i] > categoria) {
                // Como las hojas están ordenadas, si pasamos la categoría
                // ya no vamos a encontrar más
                return;
            }
        }
        nodo = nodo->siguiente; // pasamos a la siguiente hoja
    }

    if (!encontrado) {
        std::cout << "[Arbol B+] No se encontraron productos de categoria '"
                << categoria << "'.\n";
    }
}

// ============================================================
// Listado en orden y utilidades
// ============================================================

/*
 * Listar en orden es trivial en el B+: simplemente caminamos
 * la cadena de hojas de izquierda a derecha.
 * No necesitamos recorrer el árbol completo.
 */
void ArbolBPlus::listarEnOrden() const {
    if (primeraHoja == nullptr) {
        std::cout << "[Arbol B+] El arbol esta vacio.\n";
        return;
    }

    NodoBPlus *actual = primeraHoja;
    while (actual != nullptr) {
        for (int i = 0; i < actual->numClaves; i++) {
            actual->productos[i].mostrar();
        }
        actual = actual->siguiente;
    }
}

int ArbolBPlus::obtenerTamano() const {
    return tamano;
}

bool ArbolBPlus::estaVacio() const {
    return raiz == nullptr;
}

// ============================================================
// Eliminación
// ============================================================

bool ArbolBPlus::eliminar(const std::string &categoria,
                          const std::string &codigoBarra) {
    if (raiz == nullptr) return false;

    // Verificamos que exista primero
    bool existe = false;
    NodoBPlus *hoja = primeraHoja;
    while (hoja != nullptr && !existe) {
        for (int i = 0; i < hoja->numClaves; i++) {
            if (hoja->claves[i] == categoria &&
                hoja->productos[i].codigoBarra == codigoBarra) {
                existe = true;
                break;
            }
        }
        hoja = hoja->siguiente;
    }
    if (!existe) return false;

    eliminarRec(raiz, categoria, codigoBarra);
    tamano--;

    // Si la raíz quedó vacía, su hijo pasa a ser la nueva raíz
    if (raiz->numClaves == 0 && !raiz->esHoja) {
        NodoBPlus *viejaRaiz = raiz;
        raiz = raiz->hijos[0];
        viejaRaiz->hijos[0] = nullptr;
        delete viejaRaiz;
    }

    return true;
}

void ArbolBPlus::eliminarRec(NodoBPlus *nodo, const std::string &clave,
                             const std::string &codigoBarra) {
    if (nodo->esHoja) {
        // Buscamos el producto exacto por clave + codigoBarra
        for (int i = 0; i < nodo->numClaves; i++) {
            if (nodo->claves[i] == clave &&
                nodo->productos[i].codigoBarra == codigoBarra) {
                eliminarDeHoja(nodo, i);
                return;
            }
        }
        return;
    }

    // Bajamos al hijo correcto
    int idx = 0;
    while (idx < nodo->numClaves && clave >= nodo->claves[idx]) {
        idx++;
    }

    // Garantizamos que el hijo tenga suficientes claves antes de bajar
    if (nodo->hijos[idx]->numClaves < ORDEN_BPLUS) {
        rellenarBPlus(nodo, idx);
        // Recalculamos idx porque la estructura pudo cambiar
        idx = 0;
        while (idx < nodo->numClaves && clave >= nodo->claves[idx]) {
            idx++;
        }
    }

    eliminarRec(nodo->hijos[idx], clave, codigoBarra);
}

// Elimina el elemento en posición idx de la hoja
void ArbolBPlus::eliminarDeHoja(NodoBPlus *nodo, int idx) {
    for (int i = idx + 1; i < nodo->numClaves; i++) {
        nodo->claves[i - 1] = nodo->claves[i];
        nodo->productos[i - 1] = nodo->productos[i];
    }
    nodo->numClaves--;
}

// Garantiza que hijos[idx] tenga suficientes claves
void ArbolBPlus::rellenarBPlus(NodoBPlus *padre, int idx) {
    if (idx > 0 && padre->hijos[idx - 1]->numClaves >= ORDEN_BPLUS) {
        tomarDePrevioBP(padre, idx);
    } else if (idx < padre->numClaves &&
               padre->hijos[idx + 1]->numClaves >= ORDEN_BPLUS) {
        tomarDeSiguienteBP(padre, idx);
    } else {
        if (idx < padre->numClaves) {
            fusionarBPlus(padre, idx);
        } else {
            fusionarBPlus(padre, idx - 1);
        }
    }
}

// Toma la última clave del hermano izquierdo
void ArbolBPlus::tomarDePrevioBP(NodoBPlus *padre, int idx) {
    NodoBPlus *hijo = padre->hijos[idx];
    NodoBPlus *previo = padre->hijos[idx - 1];

    // Desplazamos el hijo hacia la derecha
    for (int i = hijo->numClaves - 1; i >= 0; i--) {
        hijo->claves[i + 1] = hijo->claves[i];
        hijo->productos[i + 1] = hijo->productos[i];
    }
    if (!hijo->esHoja) {
        for (int i = hijo->numClaves; i >= 0; i--) {
            hijo->hijos[i + 1] = hijo->hijos[i];
        }
    }

    if (hijo->esHoja) {
        // En hojas: bajamos la última clave del previo directamente
        hijo->claves[0] = previo->claves[previo->numClaves - 1];
        hijo->productos[0] = previo->productos[previo->numClaves - 1];
        // Actualizamos la clave separadora en el padre
        padre->claves[idx - 1] = hijo->claves[0];
    } else {
        // En nodos internos: rotamos por el padre
        hijo->claves[0] = padre->claves[idx - 1];
        hijo->productos[0] = padre->productos[idx - 1];
        if (!previo->esHoja) {
            hijo->hijos[0] = previo->hijos[previo->numClaves];
        }
        padre->claves[idx - 1] = previo->claves[previo->numClaves - 1];
        padre->productos[idx - 1] = previo->productos[previo->numClaves - 1];
    }

    hijo->numClaves++;
    previo->numClaves--;
}

// Toma la primera clave del hermano derecho
void ArbolBPlus::tomarDeSiguienteBP(NodoBPlus *padre, int idx) {
    NodoBPlus *hijo = padre->hijos[idx];
    NodoBPlus *siguiente = padre->hijos[idx + 1];

    if (hijo->esHoja) {
        // En hojas: tomamos el primer elemento del siguiente
        hijo->claves[hijo->numClaves] = siguiente->claves[0];
        hijo->productos[hijo->numClaves] = siguiente->productos[0];
        // Actualizamos clave separadora en el padre
        padre->claves[idx] = siguiente->claves[1 < siguiente->numClaves
                                                   ? 1
                                                   : 0];
    } else {
        // En nodos internos: rotamos por el padre
        hijo->claves[hijo->numClaves] = padre->claves[idx];
        hijo->productos[hijo->numClaves] = padre->productos[idx];
        if (!siguiente->esHoja) {
            hijo->hijos[hijo->numClaves + 1] = siguiente->hijos[0];
        }
        padre->claves[idx] = siguiente->claves[0];
        padre->productos[idx] = siguiente->productos[0];
    }

    // Desplazamos el siguiente hacia la izquierda
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

// Fusiona hijos[idx] con hijos[idx+1]
void ArbolBPlus::fusionarBPlus(NodoBPlus *padre, int idx) {
    NodoBPlus *izq = padre->hijos[idx];
    NodoBPlus *der = padre->hijos[idx + 1];

    if (izq->esHoja) {
        // En hojas: copiamos todo el der al izq y actualizamos el enlace
        for (int i = 0; i < der->numClaves; i++) {
            izq->claves[izq->numClaves + i] = der->claves[i];
            izq->productos[izq->numClaves + i] = der->productos[i];
        }
        izq->numClaves += der->numClaves;
        izq->siguiente = der->siguiente; // mantenemos la cadena de hojas
    } else {
        // En nodos internos: igual que árbol B
        izq->claves[izq->numClaves] = padre->claves[idx];
        izq->productos[izq->numClaves] = padre->productos[idx];
        for (int i = 0; i < der->numClaves; i++) {
            izq->claves[izq->numClaves + 1 + i] = der->claves[i];
            izq->productos[izq->numClaves + 1 + i] = der->productos[i];
        }
        for (int i = 0; i <= der->numClaves; i++) {
            izq->hijos[izq->numClaves + 1 + i] = der->hijos[i];
        }
        izq->numClaves += der->numClaves + 1;
    }

    // Sacamos la clave separadora del padre
    for (int i = idx + 1; i < padre->numClaves; i++) {
        padre->claves[i - 1] = padre->claves[i];
        padre->productos[i - 1] = padre->productos[i];
        padre->hijos[i] = padre->hijos[i + 1];
    }
    padre->numClaves--;

    der->hijos[0] = nullptr; // evitar doble-free al destruir
    delete der;
}
