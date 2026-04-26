#include "VisualizadorDot.h"
#include <iostream>
#include <sstream>

/*
 * VisualizadorDot.cpp
 * --------------------
 * Genera archivos .dot legibles por Graphviz.
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
    for (char c: s) {
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
    if (nodo == nullptr || contador >= maxNodos) return;
    contador++;

    // Truncamos el nombre a 15 chars para que quepa en el nodo
    std::string nombre = nodo->dato.nombre;
    if (nombre.size() > 15) nombre = nombre.substr(0, 13) + "..";

    // Calculamos el balance del nodo para mostrarlo
    int altIzq = (nodo->izquierda ? nodo->izquierda->altura : 0);
    int altDer = (nodo->derecha ? nodo->derecha->altura : 0);
    int balance = altIzq - altDer;

    // ID único del nodo (usamos el puntero como número)
    out << "    n" << (size_t) nodo
            << " [label=\"" << escapar(nombre)
            << "\\nalt=" << nodo->altura
            << " bal=" << balance << "\""
            << " shape=ellipse";

    // Colorear raíz de amarillo, resto blanco
    if (contador == 1) {
        out << " style=filled fillcolor=gold";
    } else {
        out << " style=filled fillcolor=lightblue";
    }
    out << "];\n";

    // Arista al hijo izquierdo
    if (nodo->izquierda && contador < maxNodos) {
        out << "    n" << (size_t) nodo
                << " -> n" << (size_t) nodo->izquierda
                << " [label=\"izq\" color=blue];\n";
        avlRec(out, nodo->izquierda, contador, maxNodos);
    }

    // Arista al hijo derecho
    if (nodo->derecha && contador < maxNodos) {
        out << "    n" << (size_t) nodo
                << " -> n" << (size_t) nodo->derecha
                << " [label=\"der\" color=darkorange];\n";
        avlRec(out, nodo->derecha, contador, maxNodos);
    }
}

bool VisualizadorDot::generarAVL(ArbolAVL *avl, int maxNodos) const {
    std::string ruta = carpetaOutput + "/avl.dot";
    std::ofstream out(ruta);
    if (!out.is_open()) {
        std::cout << "[Dot] ERROR: no se pudo crear '" << ruta << "'.\n";
        return false;
    }

    out << "// Árbol AVL — primeros " << maxNodos << " nodos\n";
    out << "// Convertir: dot -Tpng output/avl.dot -o output/avl.png\n";
    out << "digraph AVL {\n";
    out << "    graph [label=\"Arbol AVL (primeros " << maxNodos
            << " nodos)\\nClave: nombre del producto\""
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
                "gold = raiz | lightblue = nodo\\n"
                "alt = altura | bal = balance\""
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
    if (nodo == nullptr || contador >= maxNodos) return;
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
        for (int i = 0; i <= nodo->numClaves && contador < maxNodos; i++) {
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

    out << "// Árbol B (t=3) — primeros " << maxNodos << " nodos\n";
    out << "// Convertir: dot -Tpng output/arbolB.dot -o output/arbolB.png\n";
    out << "digraph ArbolB {\n";
    out << "    graph [label=\"Arbol B  t=3  (primeros " << maxNodos
            << " nodos)\\nClave: fecha de caducidad\""
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
    if (nodo == nullptr || nodo->esHoja || contador >= maxNodos) return;
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

    for (int i = 0; i <= nodo->numClaves && contador < maxNodos; i++) {
        if (nodo->hijos[i] == nullptr) continue;

        int hijoId = idNodo;
        int antesContador = contador;

        if (nodo->hijos[i]->esHoja) {
            // Arista sin puerto (plaintext no tiene puertos :fN)
            out << "    bp" << miId
                    << " -> bph" << (size_t) nodo->hijos[i]
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
    NodoBPlus *actual = primeraHoja;
    int contadorHojas = 0;

    // Usamos HTML-like labels (<<TABLE>...>) en lugar de shape=record.
    while (actual != nullptr && contadorHojas < maxNodos) {
        out << "    bph" << (size_t) actual
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
           && contadorHojas < maxNodos - 1) {
        out << "    bph" << (size_t) actual
                << " -> bph" << (size_t) actual->siguiente
                << " [style=bold color=darkgreen label=\"sig\"];\n";
        actual = actual->siguiente;
        contadorHojas++;
    }

    // Subgrafo para mantener las hojas en el mismo nivel (rank=same)
    out << "\n    { rank=same;";
    actual = primeraHoja;
    contadorHojas = 0;
    while (actual != nullptr && contadorHojas < maxNodos) {
        out << " bph" << (size_t) actual << ";";
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

    out << "// Árbol B+ (t=3) — primeros " << maxNodos << " nodos\n";
    out << "// Convertir: dot -Tpng output/arbolBP.dot -o output/arbolBP.png\n";
    out << "digraph ArbolBPlus {\n";
    out << "    graph [label=\"Arbol B+  t=3  (primeros " << maxNodos
            << " nodos)\\nClave: categoria del producto\""
            << " fontsize=14 rankdir=TB];\n";
    out << "    node  [fontname=\"Courier\" fontsize=9];\n";
    out << "    edge  [];\n\n";

    if (arbolBPlus->estaVacio()) {
        out << "    vacio [label=\"Arbol vacio\" shape=plaintext];\n";
    } else {
        int idNodo = 0, contador = 0;

        // Nodos internos
        NodoBPlus *raiz = arbolBPlus->obtenerRaiz();
        if (!raiz->esHoja) {
            arbolBPlusInternosRec(out, raiz, idNodo, contador, maxNodos / 2);
        }

        // Hojas
        out << "\n    // --- Hojas enlazadas ---\n";
        arbolBPlusHojas(out, arbolBPlus->obtenerPrimeraHoja(),
                        idNodo, maxNodos / 2);

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
