#include "ArbolAVL.h"
#include <iostream>

/*
 * ArbolAVL.cpp
 * ------------
 * Implementación completa del árbol AVL ordenado por nombre.
 *
 * Lo más importante de entender aquí son las rotaciones.
 * Cuando el árbol se desbalancea (un lado crece demasiado),
 * "rotamos" nodos para redistribuir el peso y restaurar
 * la propiedad AVL (diferencia de altura <= 1).
 *
 * Hay 4 casos de desbalanceo:
 *   LL -> rotación simple a la derecha
 *   RR -> rotación simple a la izquierda
 *   LR -> rotación doble: primero izquierda, luego derecha
 *   RL -> rotación doble: primero derecha, luego izquierda
 */

// ============================================================
// Constructor y destructor
// ============================================================

ArbolAVL::ArbolAVL() : raiz(nullptr), tamano(0) {
}

ArbolAVL::~ArbolAVL() {
    destruirArbol(raiz);
}

// Libera memoria en post-orden (primero hijos, luego el padre)
void ArbolAVL::destruirArbol(NodoAVL *nodo) {
    if (nodo == nullptr) return;
    destruirArbol(nodo->izquierda);
    destruirArbol(nodo->derecha);
    delete nodo;
}

// ============================================================
// Auxiliares de altura y balance
// ============================================================

// Si el nodo es nullptr consideramos altura 0
int ArbolAVL::obtenerAltura(NodoAVL *nodo) const {
    if (nodo == nullptr) return 0;
    return nodo->altura;
}

// Balance = altura izquierda - altura derecha
// Si es > 1 -> cargado a la izquierda
// Si es < -1 -> cargado a la derecha
int ArbolAVL::obtenerBalance(NodoAVL *nodo) const {
    if (nodo == nullptr) return 0;
    return obtenerAltura(nodo->izquierda) - obtenerAltura(nodo->derecha);
}

// La altura de un nodo es 1 + la altura del hijo más alto
void ArbolAVL::actualizarAltura(NodoAVL *nodo) {
    if (nodo == nullptr) return;
    int altIzq = obtenerAltura(nodo->izquierda);
    int altDer = obtenerAltura(nodo->derecha);
    nodo->altura = 1 + (altIzq > altDer ? altIzq : altDer);
}

// ============================================================
// Rotaciones
// ============================================================

// Rotación simple a la DERECHA (caso LL)
NodoAVL *ArbolAVL::rotarDerecha(NodoAVL *y) {
    NodoAVL *x = y->izquierda;
    NodoAVL *T2 = x->derecha;

    // Hacemos la rotación
    x->derecha = y;
    y->izquierda = T2;

    // Actualizamos alturas — primero y (ahora es hijo), luego x (nueva raíz)
    actualizarAltura(y);
    actualizarAltura(x);

    return x; // x es la nueva raíz de este subárbol
}

// Rotación simple a la IZQUIERDA (caso RR)
NodoAVL *ArbolAVL::rotarIzquierda(NodoAVL *x) {
    NodoAVL *y = x->derecha;
    NodoAVL *T1 = y->izquierda;

    // Hacemos la rotación
    y->izquierda = x;
    x->derecha = T1;

    actualizarAltura(x);
    actualizarAltura(y);

    return y; // y es la nueva raíz de este subárbol
}

/*
 * Rebalancear — revisa el factor de balance y aplica
 * la rotación correspondiente si es necesario.
 *
 * Los 4 casos:
 *  balance > 1  y hijo izq. balanceado a la izq. -> LL -> rotarDerecha
 *  balance > 1  y hijo izq. balanceado a la der. -> LR -> rotarIzquierda(izq) + rotarDerecha
 *  balance < -1 y hijo der. balanceado a la der. -> RR -> rotarIzquierda
 *  balance < -1 y hijo der. balanceado a la izq. -> RL -> rotarDerecha(der) + rotarIzquierda
 */
NodoAVL *ArbolAVL::rebalancear(NodoAVL *nodo) {
    actualizarAltura(nodo);
    int balance = obtenerBalance(nodo);

    // Caso LL
    if (balance > 1 && obtenerBalance(nodo->izquierda) >= 0) {
        return rotarDerecha(nodo);
    }
    // Caso LR
    if (balance > 1 && obtenerBalance(nodo->izquierda) < 0) {
        nodo->izquierda = rotarIzquierda(nodo->izquierda);
        return rotarDerecha(nodo);
    }
    // Caso RR
    if (balance < -1 && obtenerBalance(nodo->derecha) <= 0) {
        return rotarIzquierda(nodo);
    }
    // Caso RL
    if (balance < -1 && obtenerBalance(nodo->derecha) > 0) {
        nodo->derecha = rotarDerecha(nodo->derecha);
        return rotarIzquierda(nodo);
    }

    return nodo; // ya estaba balanceado, no hacemos nada
}

// ============================================================
// Inserción
// ============================================================

// Método público — llama al recursivo desde la raíz
void ArbolAVL::insertar(const Producto &producto) {
    raiz = insertarRec(raiz, producto);
    tamano++;
}

// Baja recursivamente hasta encontrar el lugar donde insertar,
// luego al volver sube rebalanceando cada nodo del camino
NodoAVL *ArbolAVL::insertarRec(NodoAVL *nodo, const Producto &producto) {
    // Caso base: lugar vacío, creamos el nodo aquí
    if (nodo == nullptr) {
        return new NodoAVL(producto);
    }

    // Bajamos según el orden alfabético del nombre
    if (producto.nombre < nodo->dato.nombre) {
        nodo->izquierda = insertarRec(nodo->izquierda, producto);
    } else if (producto.nombre > nodo->dato.nombre) {
        nodo->derecha = insertarRec(nodo->derecha, producto);
    } else {
        // Nombre duplicado — no insertamos (el nombre es clave del AVL)
        tamano--; // compensamos el ++ del método público
        return nodo;
    }

    return rebalancear(nodo);
}

// ============================================================
// Búsqueda
// ============================================================

Producto *ArbolAVL::buscar(const std::string &nombre) const {
    return buscarRec(raiz, nombre);
}

// Búsqueda binaria clásica: comparamos y bajamos por el lado correcto
Producto *ArbolAVL::buscarRec(NodoAVL *nodo, const std::string &nombre) const {
    if (nodo == nullptr) return nullptr; // no encontrado

    if (nombre == nodo->dato.nombre) {
        return &nodo->dato; // encontrado
    } else if (nombre < nodo->dato.nombre) {
        return buscarRec(nodo->izquierda, nombre);
    } else {
        return buscarRec(nodo->derecha, nombre);
    }
}

// ============================================================
// Eliminación
// ============================================================

bool ArbolAVL::eliminar(const std::string &nombre) {
    // Verificamos que exista antes de intentar eliminar
    if (buscar(nombre) == nullptr) return false;
    raiz = eliminarRec(raiz, nombre);
    tamano--;
    return true;
}

// El nodo más a la izquierda de un subárbol es el de menor valor
NodoAVL *ArbolAVL::obtenerMinimo(NodoAVL *nodo) const {
    NodoAVL *actual = nodo;
    while (actual->izquierda != nullptr) {
        actual = actual->izquierda;
    }
    return actual;
}

/*
 * Eliminación con rebalanceo:
 * 1. Buscamos el nodo a eliminar
 * 2. Si tiene 0 o 1 hijo -> fácil, simplemente lo sacamos
 * 3. Si tiene 2 hijos -> lo reemplazamos con su sucesor in-order
 *    (el mínimo del subárbol derecho) y eliminamos ese sucesor
 * 4. Al volver de la recursión rebalanceamos cada nodo del camino
 */
NodoAVL *ArbolAVL::eliminarRec(NodoAVL *nodo, const std::string &nombre) {
    if (nodo == nullptr) return nullptr;

    if (nombre < nodo->dato.nombre) {
        nodo->izquierda = eliminarRec(nodo->izquierda, nombre);
    } else if (nombre > nodo->dato.nombre) {
        nodo->derecha = eliminarRec(nodo->derecha, nombre);
    } else {
        // Encontramos el nodo a eliminar
        if (nodo->izquierda == nullptr || nodo->derecha == nullptr) {
            // 0 o 1 hijo: lo reemplazamos con el hijo que existe (o nullptr)
            NodoAVL *hijo = (nodo->izquierda != nullptr)
                                ? nodo->izquierda
                                : nodo->derecha;
            delete nodo;
            return hijo; // puede ser nullptr si no tenía hijos
        }

        // 2 hijos: buscamos el sucesor in-order (mínimo del subárbol derecho)
        NodoAVL *sucesor = obtenerMinimo(nodo->derecha);

        // Copiamos los datos del sucesor a este nodo
        nodo->dato = sucesor->dato;

        // Eliminamos el sucesor del subárbol derecho
        nodo->derecha = eliminarRec(nodo->derecha, sucesor->dato.nombre);
    }

    return rebalancear(nodo);
}

// ============================================================
// Listado in-order y utilidades
// ============================================================

// Recorrido in-order: izquierda -> raíz -> derecha
// Esto nos da los productos en orden alfabético automáticamente
void ArbolAVL::listarEnOrden() const {
    if (raiz == nullptr) {
        std::cout << "[AVL] El arbol esta vacio.\n";
        return;
    }
    listarRec(raiz);
}

void ArbolAVL::listarRec(NodoAVL *nodo) const {
    if (nodo == nullptr) return;
    listarRec(nodo->izquierda);
    nodo->dato.mostrar();
    listarRec(nodo->derecha);
}

int ArbolAVL::obtenerTamano() const {
    return tamano;
}

bool ArbolAVL::estaVacio() const {
    return raiz == nullptr;
}
