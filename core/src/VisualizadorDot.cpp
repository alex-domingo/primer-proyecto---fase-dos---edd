#include "VisualizadorDot.h"
#include <queue>
#include <iomanip>
#include <iostream>
#include <sstream>

/*
 * VisualizadorDot.cpp
 * --------------------
 * Genera archivos .dot legibles por Graphviz.
 *
 * Formato DOT básico:
 *   digraph G {
 *       node [shape=record];   // para nodos con múltiples campos
 *       A [label="texto"];
 *       A -> B;
 *   }
 *
 * Para el AVL usamos shape=ellipse con el nombre del producto truncado.
 * Para el Árbol B y B+ usamos shape=record con celdas separadas por |
 * que permiten ver claramente las múltiples claves por nodo.
 *
 * El parámetro maxNodos limita cuántos nodos dibujamos para
 * mantener la imagen legible (30 nodos AVL ≈ 4-5 niveles visibles).
 */

VisualizadorDot::VisualizadorDot(const std::string &carpeta)
    : carpetaOutput(carpeta) {
}

// Escapa caracteres que romperían el formato DOT
std::string VisualizadorDot::escapar(const std::string &s) const {
    std::string resultado;
    for (char c : s) {
        if (c == '"' || c == '\\' || c == '<' || c == '>' ||
            c == '{' || c == '}' || c == '|') {
            resultado += '\\';
        }
        resultado += c;
    }
    return resultado;
}

// ============================================================
// ÁRBOL AVL
// ============================================================

/*
 * Recorrido pre-orden del AVL.
 * Por cada nodo emitimos:
 *   - El nodo con su etiqueta (nombre truncado + altura + balance)
 *   - Las aristas hacia sus hijos
 *
 * Los nodos izquierdos son azules y los derechos naranja,
 * para que sea fácil seguir la estructura visualmente.
 */
void VisualizadorDot::avlRec(std::ofstream &out, NodoAVL *nodo,
                             int &contador, int maxNodos) const {
    // Recorrido por NIVELES (BFS) en vez de preorden.
    //
    // Antes usábamos preorden con un límite de nodos, lo que gastaba
    // todo el presupuesto en la rama izquierda y dejaba sin dibujar las
    // ramas derechas de los nodos superiores. Eso daba la ILUSIÓN de un
    // árbol en cadena lineal aunque estuviera balanceado.
    //
    // Con BFS dibujamos nivel por nivel: si hay que cortar por el límite,
    // se corta abajo de forma pareja, preservando la forma real del árbol.
    if (nodo == nullptr) return;

    // maxNodos == 0 significa SIN LÍMITE: usamos un tope efectivamente infinito
    bool sinLimite = (maxNodos <= 0);

    std::queue<NodoAVL*> cola;
    cola.push(nodo);

    while (!cola.empty() && (sinLimite || contador < maxNodos)) {
        NodoAVL *actual = cola.front();
        cola.pop();
        contador++;

        // Nombre truncado a 15 chars
        std::string nombre = actual->dato.nombre;
        if (nombre.size() > 15) nombre = nombre.substr(0, 13) + "..";

        int altIzq = (actual->izquierda ? actual->izquierda->altura : 0);
        int altDer = (actual->derecha  ? actual->derecha->altura  : 0);
        int balance = altIzq - altDer;

        // Detectar desbalance real: si |balance| > 1 el AVL estaría roto.
        // Lo pintamos de rojo para que sea evidente (en un AVL sano no ocurre).
        bool desbalanceado = (balance < -1 || balance > 1);

        out << "    n" << (size_t)actual
            << " [label=\"" << escapar(nombre)
            << "\\nalt=" << actual->altura
            << " bal=" << balance << "\""
            << " shape=ellipse style=filled fillcolor=";
        if (contador == 1)        out << "gold";       // raíz
        else if (desbalanceado)   out << "\"#FF6B6B\""; // rojo: nodo roto
        else                      out << "lightblue";  // nodo sano
        out << "];\n";

        // Encolar hijos y dibujar sus aristas
        if (actual->izquierda && (sinLimite || contador < maxNodos)) {
            out << "    n" << (size_t)actual
                << " -> n" << (size_t)actual->izquierda
                << " [label=\"izq\" color=blue];\n";
            cola.push(actual->izquierda);
        }
        if (actual->derecha && (sinLimite || contador < maxNodos)) {
            out << "    n" << (size_t)actual
                << " -> n" << (size_t)actual->derecha
                << " [label=\"der\" color=darkorange];\n";
            cola.push(actual->derecha);
        }
    }
}

bool VisualizadorDot::generarAVL(ArbolAVL *avl, int maxNodos) const {
    std::string ruta = carpetaOutput + "/avl.dot";
    std::ofstream out(ruta);
    if (!out.is_open()) {
        std::cout << "[Dot] ERROR: no se pudo crear '" << ruta << "'.\n";
        return false;
    }

    out << "// Árbol AVL completo\n";
    out << "// Convertir: dot -Tpng output/avl.dot -o output/avl.png\n";
    out << "digraph AVL {\n";
    out << "    graph [label=\"Arbol AVL completo\\nClave: nombre del producto\""
        << " fontsize=14 rankdir=TB];\n";
    out << "    node  [fontname=\"Helvetica\" fontsize=10];\n";
    out << "    edge  [fontsize=8];\n\n";

    if (avl->estaVacio()) {
        out << "    vacio [label=\"Arbol vacio\" shape=plaintext];\n";
    } else {
        int contador = 0;
        avlRec(out, avl->obtenerRaiz(), contador, maxNodos);
        out << "\n    // Leyenda\n";
        out << "    leyenda [label=\"Leyenda:\\n"
               "gold = raiz | lightblue = nodo sano\\n"
               "rojo = desbalanceado | alt = altura | bal = balance\""
               " shape=note style=filled fillcolor=lightyellow];\n";
    }

    out << "}\n";
    out.close();

    std::cout << "[Dot] Generado: " << ruta << "\n";
    return true;
}

// ============================================================
// ÁRBOL B
// ============================================================

/*
 * En el árbol B cada nodo tiene VARIAS claves.
 * Usamos shape=record para representarlos como una tabla horizontal:
 *   [hijo0 | clave0 | hijo1 | clave1 | hijo2 | ...]
 *
 * Las claves son fechas de caducidad (YYYY-MM-DD).
 * Los nodos hoja se colorean verde, los internos azul claro.
 */
void VisualizadorDot::arbolBRec(std::ofstream &out, NodoB *nodo,
                                int &idNodo, int &contador,
                                int maxNodos) const {
    bool sinLimite = (maxNodos <= 0);
    if (nodo == nullptr || (!sinLimite && contador >= maxNodos)) return;
    contador++;

    int miId = idNodo++;

    // HTML-like label: cada clave en su propia celda (más robusto que record)
    std::string bgColor = nodo->esHoja ? "palegreen" : "lightblue";
    out << "    b" << miId
        << " [label=<<TABLE BORDER=\"0\" CELLBORDER=\"1\""
           " CELLSPACING=\"0\" BGCOLOR=\"" << bgColor << "\"><TR>";
    for (int i = 0; i < nodo->numClaves; i++) {
        // Fecha corta: "2026-05" en lugar del año completo para que quepa
        std::string fecha = nodo->claves[i];
        if (fecha.size() > 7) fecha = fecha.substr(0, 7);
        out << "<TD>" << escapar(fecha) << "</TD>";
    }
    out << "</TR></TABLE>> shape=plaintext];\n";

    // Recursión en hijos con aristas (sin puertos :fN — plaintext no los tiene)
    if (!nodo->esHoja) {
        for (int i = 0; i <= nodo->numClaves && (sinLimite || contador < maxNodos); i++) {
            if (nodo->hijos[i] == nullptr) continue;

            int hijoContadorAntes = contador;
            int hijoId = idNodo;
            arbolBRec(out, nodo->hijos[i], idNodo, contador, maxNodos);

            if (contador > hijoContadorAntes) {
                out << "    b" << miId << " -> b" << hijoId << ";\n";
            }
        }
    }
}

bool VisualizadorDot::generarArbolB(ArbolB *arbolB, int maxNodos) const {
    std::string ruta = carpetaOutput + "/arbolB.dot";
    std::ofstream out(ruta);
    if (!out.is_open()) {
        std::cout << "[Dot] ERROR: no se pudo crear '" << ruta << "'.\n";
        return false;
    }

    out << "// Árbol B (t=3) completo\n";
    out << "// Convertir: dot -Tpng output/arbolB.dot -o output/arbolB.png\n";
    out << "digraph ArbolB {\n";
    out << "    graph [label=\"Arbol B  t=3  completo\\nClave: fecha de caducidad\""
        << " fontsize=14 rankdir=TB];\n";
    out << "    node  [fontname=\"Courier\" fontsize=9];\n";
    out << "    edge  [];\n\n";

    if (arbolB->estaVacio()) {
        out << "    vacio [label=\"Arbol vacio\" shape=plaintext];\n";
    } else {
        int idNodo = 0, contador = 0;
        arbolBRec(out, arbolB->obtenerRaiz(), idNodo, contador, maxNodos);
        out << "\n    leyenda [label=\"verde=hoja | azul=interno\""
               " shape=note style=filled fillcolor=lightyellow];\n";
    }

    out << "}\n";
    out.close();

    std::cout << "[Dot] Generado: " << ruta << "\n";
    return true;
}

// ============================================================
// ÁRBOL B+
// ============================================================

/*
 * El árbol B+ tiene dos partes:
 *   1. Nodos internos (solo claves de navegación) → azul claro
 *   2. Hojas (datos reales, enlazadas en cadena) → verde
 *
 * Generamos primero los nodos internos recursivamente,
 * luego recorremos la cadena de hojas y añadimos aristas
 * horizontales entre ellas (el enlace 'siguiente').
 *
 * Las claves son categorías de productos.
 */
void VisualizadorDot::arbolBPlusInternosRec(std::ofstream &out,
                                            NodoBPlus *nodo,
                                            int &idNodo, int &contador,
                                            int maxNodos) const {
    bool sinLimite = (maxNodos <= 0);
    if (nodo == nullptr || nodo->esHoja || (!sinLimite && contador >= maxNodos)) return;
    contador++;

    int miId = idNodo++;

    // HTML-like label para nodos internos (evita el bug de record+flat edges)
    out << "    bp" << miId
        << " [label=<<TABLE BORDER=\"0\" CELLBORDER=\"1\""
           " CELLSPACING=\"0\" BGCOLOR=\"lightblue\"><TR>";
    for (int i = 0; i < nodo->numClaves; i++) {
        std::string clave = nodo->claves[i];
        if (clave.size() > 9) clave = clave.substr(0, 8) + ".";
        out << "<TD>" << escapar(clave) << "</TD>";
    }
    out << "</TR></TABLE>> shape=plaintext];\n";

    for (int i = 0; i <= nodo->numClaves && (sinLimite || contador < maxNodos); i++) {
        if (nodo->hijos[i] == nullptr) continue;

        int hijoId = idNodo;
        int antesContador = contador;

        if (nodo->hijos[i]->esHoja) {
            // Arista sin puerto (plaintext no tiene puertos :fN)
            out << "    bp" << miId
                << " -> bph" << (size_t)nodo->hijos[i]
                << " [style=dashed];\n";
        } else {
            arbolBPlusInternosRec(out, nodo->hijos[i], idNodo, contador, maxNodos);
            if (contador > antesContador) {
                out << "    bp" << miId << " -> bp" << hijoId << ";\n";
            }
        }
    }
}

void VisualizadorDot::arbolBPlusHojas(std::ofstream &out,
                                      NodoBPlus *primeraHoja,
                                      int &idNodo, int maxNodos) const {
    bool sinLimite = (maxNodos <= 0);
    NodoBPlus *actual = primeraHoja;
    int contadorHojas = 0;

    /*
     * Usamos HTML-like labels (<<TABLE>...>) en lugar de shape=record.
     * Graphviz falla con aristas horizontales entre nodos record del mismo
     * rango ("flat edge between adjacent nodes with record shape").
     * Los HTML-like labels evitan ese bug completamente.
     */
    while (actual != nullptr && (sinLimite || contadorHojas < maxNodos)) {
        out << "    bph" << (size_t)actual
            << " [label=<<TABLE BORDER=\"0\" CELLBORDER=\"1\""
               " CELLSPACING=\"0\" BGCOLOR=\"palegreen\">"
               "<TR>";

        for (int i = 0; i < actual->numClaves; i++) {
            std::string clave = actual->claves[i];
            if (clave.size() > 9) clave = clave.substr(0, 8) + ".";
            out << "<TD>" << escapar(clave) << "</TD>";
        }

        out << "</TR></TABLE>> shape=plaintext];\n";

        actual = actual->siguiente;
        contadorHojas++;
        idNodo++;
    }

    // Luego las aristas horizontales entre hojas (el enlace siguiente)
    actual = primeraHoja;
    contadorHojas = 0;
    while (actual != nullptr && actual->siguiente != nullptr
           && (sinLimite || contadorHojas < maxNodos - 1)) {
        out << "    bph" << (size_t)actual
            << " -> bph" << (size_t)actual->siguiente
            << " [style=bold color=darkgreen label=\"sig\"];\n";
        actual = actual->siguiente;
        contadorHojas++;
    }

    // Subgrafo para mantener las hojas en el mismo nivel (rank=same)
    out << "\n    { rank=same;";
    actual = primeraHoja;
    contadorHojas = 0;
    while (actual != nullptr && (sinLimite || contadorHojas < maxNodos)) {
        out << " bph" << (size_t)actual << ";";
        actual = actual->siguiente;
        contadorHojas++;
    }
    out << " }\n";
}

bool VisualizadorDot::generarArbolBPlus(ArbolBPlus *arbolBPlus,
                                        int maxNodos) const {
    std::string ruta = carpetaOutput + "/arbolBP.dot";
    std::ofstream out(ruta);
    if (!out.is_open()) {
        std::cout << "[Dot] ERROR: no se pudo crear '" << ruta << "'.\n";
        return false;
    }

    out << "// Árbol B+ (t=3) completo\n";
    out << "// Convertir: dot -Tpng output/arbolBP.dot -o output/arbolBP.png\n";
    out << "digraph ArbolBPlus {\n";
    out << "    graph [label=\"Arbol B+  t=3  completo\\nClave: categoria del producto\""
        << " fontsize=14 rankdir=TB];\n";
    out << "    node  [fontname=\"Courier\" fontsize=9];\n";
    out << "    edge  [];\n\n";

    if (arbolBPlus->estaVacio()) {
        out << "    vacio [label=\"Arbol vacio\" shape=plaintext];\n";
    } else {
        int idNodo = 0, contador = 0;

        // Si maxNodos es 0 (sin límite), pasamos 0 a ambos para que no
        // limiten. Si hay límite, lo repartimos entre internos y hojas.
        int limInternos = (maxNodos <= 0) ? 0 : maxNodos / 2;
        int limHojas    = (maxNodos <= 0) ? 0 : maxNodos / 2;

        // Nodos internos
        NodoBPlus *raiz = arbolBPlus->obtenerRaiz();
        if (!raiz->esHoja) {
            arbolBPlusInternosRec(out, raiz, idNodo, contador, limInternos);
        }

        // Hojas
        out << "\n    // --- Hojas enlazadas ---\n";
        arbolBPlusHojas(out, arbolBPlus->obtenerPrimeraHoja(),
                        idNodo, limHojas);

        out << "\n    leyenda [label=\""
               "azul=nodo interno\\nverde=hoja\\n"
               "bold green=enlace siguiente\""
               " shape=note style=filled fillcolor=lightyellow];\n";
    }

    out << "}\n";
    out.close();

    std::cout << "[Dot] Generado: " << ruta << "\n";
    return true;
}

// ============================================================
// Generar los tres de una vez
// ============================================================

void VisualizadorDot::generarTodos(ArbolAVL *avl, ArbolB *arbolB,
                                   ArbolBPlus *arbolBPlus) const {
    std::cout << "\n=========================================\n";
    std::cout << "     Fase 8 - Visualizacion Graphviz    \n";
    std::cout << "=========================================\n";

    bool ok1 = generarAVL(avl, 30);
    bool ok2 = generarArbolB(arbolB, 20);
    bool ok3 = generarArbolBPlus(arbolBPlus, 20);

    if (ok1 && ok2 && ok3) {
        std::cout << "\nTodos los archivos .dot generados en '"
                  << carpetaOutput << "/'.\n\n";
        std::cout << "Para convertir a PNG (requiere Graphviz instalado):\n";
        std::cout << "  dot -Tpng output/avl.dot    -o output/avl.png\n";
        std::cout << "  dot -Tpng output/arbolB.dot -o output/arbolB.png\n";
        std::cout << "  dot -Tpng output/arbolBP.dot -o output/arbolBP.png\n";
        std::cout << "\nEn Windows con Graphviz instalado, desde la terminal:\n";
        std::cout << "  cd <ruta_del_proyecto>\n";
        std::cout << "  dot -Tpng output\\avl.dot -o output\\avl.png\n";
    }
}

// ════════════════════════════════════════════════════════════
// Visualización de la Tabla Hash (colisiones + factor de carga)
// ════════════════════════════════════════════════════════════
bool VisualizadorDot::generarHash(const TablaHash *hash, bool soloConContenido) const {
    std::string ruta = carpetaOutput + "/hash.dot";
    std::ofstream out(ruta);
    if (!out.is_open()) {
        std::cout << "[Dot] ERROR: no se pudo crear '" << ruta << "'.\n";
        return false;
    }

    int capacidad  = hash->obtenerCapacidad();
    int elementos  = hash->obtenerTamano();
    double fc      = hash->factorCarga();

    // Calcular estadísticas de distribución para mostrarlas
    int bucketsUsados = 0;
    int maxCadena = 0;
    int conColision = 0; // buckets con 2+ elementos
    for (int i = 0; i < capacidad; i++) {
        int largo = 0;
        for (NodoHash *n = hash->obtenerBucket(i); n != nullptr; n = n->siguiente)
            largo++;
        if (largo > 0) bucketsUsados++;
        if (largo > 1) conColision++;
        if (largo > maxCadena) maxCadena = largo;
    }

    out << "// Tabla Hash — buckets, colisiones y factor de carga\n";
    out << "// Convertir: dot -Tpng output/hash.dot -o output/hash.png\n";
    out << "digraph TablaHash {\n";
    out << "    rankdir=LR;\n"; // izquierda a derecha: bucket -> cadena
    out << "    graph [label=\"Tabla Hash (chaining, djb2)\\n"
        << "Capacidad: " << capacidad << " buckets  |  "
        << "Elementos: " << elementos << "\\n"
        << "Factor de carga: " << std::fixed << std::setprecision(3) << fc
        << "  |  Buckets usados: " << bucketsUsados << "/" << capacidad << "\\n"
        << "Buckets con colision: " << conColision
        << "  |  Cadena mas larga: " << maxCadena << "\""
        << " fontsize=14 labelloc=t];\n";
    out << "    node [fontname=\"Courier\" fontsize=9];\n\n";

    // Columna de índices del arreglo (los buckets)
    for (int i = 0; i < capacidad; i++) {
        NodoHash *cabeza = hash->obtenerBucket(i);
        bool vacio = (cabeza == nullptr);

        // Si soloConContenido, saltamos los buckets vacíos
        if (soloConContenido && vacio) continue;

        // Color del bucket según su estado
        std::string color;
        int largo = 0;
        for (NodoHash *n = cabeza; n; n = n->siguiente) largo++;
        if (vacio)        color = "white";
        else if (largo == 1) color = "lightblue";   // sin colisión
        else              color = "\"#FFD54F\"";     // amarillo: colisión

        out << "    bucket" << i
            << " [label=\"[" << i << "]\" shape=square style=filled fillcolor="
            << color << " width=0.5];\n";

        // Dibujar la cadena de nodos (productos) de este bucket
        std::string anterior = "bucket" + std::to_string(i);
        int idx = 0;
        for (NodoHash *n = cabeza; n != nullptr; n = n->siguiente) {
            std::string nombre = n->dato.nombre;
            if (nombre.size() > 14) nombre = nombre.substr(0, 12) + "..";
            std::string codigo = n->dato.codigoBarra;

            std::string nodeId = "h" + std::to_string(i) + "_" + std::to_string(idx);
            out << "    " << nodeId
                << " [label=\"" << escapar(nombre) << "\\n" << escapar(codigo)
                << "\" shape=box style=filled fillcolor=palegreen];\n";
            out << "    " << anterior << " -> " << nodeId
                << (idx == 0 ? " [color=black];\n" : " [color=red label=\"col\"];\n");
            anterior = nodeId;
            idx++;
        }
    }

    // Mantener los buckets alineados verticalmente (rank=same por columna)
    out << "\n    // Alinear buckets en columna\n";
    out << "    { rank=same;";
    // (los buckets ya van en columna por rankdir=LR; este subgrafo refuerza)
    out << " }\n";

    // Leyenda
    out << "\n    leyenda [label=\""
        << "azul=bucket sin colision\\n"
        << "amarillo=bucket con colision\\n"
        << "verde=producto  |  rojo=colision (chaining)\""
        << " shape=note style=filled fillcolor=lightyellow];\n";

    out << "}\n";
    out.close();
    std::cout << "[Dot] Generado: " << ruta << "\n";
    return true;
}
