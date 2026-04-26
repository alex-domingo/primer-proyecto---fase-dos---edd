#ifndef PROYECTO_EDD_VISUALIZADORDOT_H
#define PROYECTO_EDD_VISUALIZADORDOT_H

#include "NodoAVL.h"
#include "NodoB.h"
#include "NodoBPlus.h"
#include "ArbolAVL.h"
#include "ArbolB.h"
#include "ArbolBPlus.h"
#include <string>
#include <fstream>

/*
 * VisualizadorDot.h
 * ------------------
 * Genera archivos .dot (formato Graphviz) a partir de las
 * estructuras de árbol del catálogo.
 *
 * Cada método genera un archivo en output/ y retorna true si tuvo éxito.
 *
 * Para convertir los .dot a imagen PNG desde terminal:
 *   dot -Tpng output/avl.dot     -o output/avl.png
 *   dot -Tpng output/arbolB.dot  -o output/arbolB.png
 *   dot -Tpng output/arbolBP.dot -o output/arbolBP.png
 *
 * Nota sobre escala:
 *   Con 1050 productos los árboles completos son ilegibles.
 *   Generamos una muestra de los primeros 30 nodos de cada árbol
 *   para que la imagen sea legible y pedagógica.
 *   El parámetro maxNodos controla esto.
 */

class VisualizadorDot {
private:
    std::string carpetaOutput; // ruta de la carpeta output/

    // ---- AVL ----
    // Recorre el AVL en pre-orden emitiendo nodos y aristas
    void avlRec(std::ofstream &out, NodoAVL *nodo,
                int &contador, int maxNodos) const;

    // ---- Árbol B ----
    // Recorre el árbol B emitiendo registros de tabla (nodos multi-clave)
    void arbolBRec(std::ofstream &out, NodoB *nodo,
                   int &idNodo, int &contador, int maxNodos) const;

    // ---- Árbol B+ ----
    // Recorre nodos internos del árbol B+
    void arbolBPlusInternosRec(std::ofstream &out, NodoBPlus *nodo,
                               int &idNodo, int &contador,
                               int maxNodos) const;

    // Recorre las hojas enlazadas del árbol B+
    void arbolBPlusHojas(std::ofstream &out, NodoBPlus *primeraHoja,
                         int &idNodo, int maxNodos) const;

    // Escapa caracteres especiales para etiquetas DOT
    std::string escapar(const std::string &s) const;

public:
    explicit VisualizadorDot(const std::string &carpeta = "output");

    // Genera output/avl.dot con los primeros maxNodos nodos del AVL
    bool generarAVL(ArbolAVL *avl, int maxNodos = 30) const;

    // Genera output/arbolB.dot con los primeros maxNodos nodos del Árbol B
    bool generarArbolB(ArbolB *arbolB, int maxNodos = 20) const;

    // Genera output/arbolBP.dot con los primeros maxNodos nodos del Árbol B+
    bool generarArbolBPlus(ArbolBPlus *arbolBPlus, int maxNodos = 20) const;

    // Genera los tres archivos de una vez y muestra instrucciones
    void generarTodos(ArbolAVL *avl, ArbolB *arbolB,
                      ArbolBPlus *arbolBPlus) const;
};

#endif //PROYECTO_EDD_VISUALIZADORDOT_H
