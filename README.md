# Gestión de Catálogo de Productos de Supermercado — Fase 2

Curso de Estructura de Datos

---

## Descripción

Sistema de gestión de inventario multi-sucursal con interfaz gráfica Qt.
La red de sucursales está modelada como un **grafo ponderado** con algoritmo **Dijkstra**.
Cada sucursal administra su propio catálogo con **6 estructuras de datos** sincronizadas.

---

## Estructura del proyecto

```
edd-proyecto-fase-2-gui/
├── edd-proyecto-fase-2-gui.pro   ← archivo de proyecto Qt
├── main.cpp                      ← punto de entrada
├── mainwindow.h / .cpp / .ui     ← ventana principal (7 tabs)
│
├── core/                         ← Fase 1 (sin cambios estructurales)
│   ├── include/                  ← 16 headers
│   └── src/                      ← 16 implementaciones
│
├── include/                      ← Clases nuevas Fase 2
│   ├── Cola.h                    ← FIFO genérica (template)
│   ├── Pila.h                    ← LIFO genérica (template)
│   ├── Operacion.h               ← registro de historial
│   ├── Sucursal.h                ← nodo de la red
│   ├── RedSucursales.h           ← grafo + Dijkstra
│   ├── CargadorRed.h             ← carga de 3 CSV
│   └── SimuladorTransferencia.h  ← máquina de estados Qt
│
├── src/
│   ├── Sucursal.cpp
│   ├── RedSucursales.cpp
│   ├── CargadorRed.cpp
│   └── SimuladorTransferencia.cpp
│
├── data/
│   ├── sucursales.csv            ← 7 sucursales guatemaltecas
│   ├── conexiones.csv            ← 10 conexiones con tiempo y costo
│   └── productos_fase2.csv      ← 1050 productos distribuidos
│
├── output/                       ← archivos .dot y .png generados
└── docs/                         ← documentación del proyecto
```

---

## Compilar y ejecutar

### Requisitos
- Qt Creator 6.x con MinGW 64-bit (Windows) o GCC (Linux)
- C++17

### Opción A — Qt Creator
1. Abrir `edd-proyecto-fase-2-gui.pro`
2. Configurar el kit (MinGW 64-bit recomendado)
3. Configurar el Working Directory en `$PROJECT_DIR$`
4. Presionar **Run** (`Ctrl+R`)

> **Working Directory es obligatorio** para que el programa encuentre la carpeta `data/` al cargar los CSV.

---

## Uso del sistema

Al iniciar, el sistema arranca con la **red vacía**. Usar los botones del **Tab Sistema** para cargar datos.

### Tabs disponibles

| Tab | Funcionalidad |
|-----|---------------|
| Sistema | Carga de CSV, estadísticas, estado de estructuras |
| Sucursales | CRUD de sucursales, inventario por sucursal |
| Red | Visualización del grafo, CRUD de conexiones |
| Inventario | Búsqueda global con 4 criterios, agregar/eliminar productos |
| Transferencia | Cálculo de ruta óptima, simulación animada |
| Rendimiento | Benchmark de 4 estructuras (Lista Simple, Lista Ordenada, AVL, Hash) |
| Visualización | Generación de archivos .dot y PNG con Graphviz |

### Carga de datos

El Tab Sistema ofrece cuatro opciones:

- **Cargar Sucursales** — abre `sucursales.csv`
- **Cargar Conexiones** — abre `conexiones.csv` (requiere sucursales previas)
- **Cargar Productos** — abre `productos_fase2.csv`
- **⚡ Cargar Todo** — carga los tres CSV default de una vez

### Transferencia de productos

1. Seleccionar sucursal origen y destino
2. Ingresar el código de barra del producto (10 dígitos)
3. Elegir criterio: minimizar tiempo o costo
4. Clic en **Calcular ruta óptima** — muestra el camino con pesos
5. Ajustar las unidades a transferir
6. Elegir velocidad de simulación (1x a 120x)
7. Clic en **Simular transferencia** — animación en tiempo real

---

## Estructuras de datos

### Fase 1 (por sucursal)

| Estructura | Clave | Complejidad |
|-----------|-------|-------------|
| Lista Simple | — | O(1) inserción, O(n) búsqueda |
| Lista Ordenada | Nombre | O(n) inserción, O(n) búsqueda* |
| Árbol AVL | Nombre | O(log n) |
| Árbol B (t=3) | Fecha caducidad | O(log n+k) rango |
| Árbol B+ (t=3) | Categoría | O(log n+k) categoría |
| Tabla Hash | Código de barra | O(1) amortizado |

\* Con corte anticipado

### Fase 2 (nuevas)

| Estructura | Uso | Complejidad |
|-----------|-----|-------------|
| Cola\<T\> | Flujo de productos por sucursal | O(1) todas |
| Pila\<T\> | Historial de operaciones (rollback) | O(1) todas |
| Grafo (lista adyacencia) | Red de sucursales | O(V+E) recorrido |
| Dijkstra | Ruta óptima tiempo/costo | O((V+E) log V) |

---

## Formato de los CSV

### sucursales.csv
```
"ID","Nombre","Ubicacion","TiempoIngreso","TiempoTraspaso","TiempoDespacho"
"SUC01","Central Zona 1","Ciudad de Guatemala, Zona 1",30,45,20
```

### conexiones.csv
```
"OrigenID","DestinoID","Tiempo","Costo","Bidireccional"
"SUC01","SUC02",25,15.50,true
```

### productos_fase2.csv
```
"SucursalID","Nombre","CodigoBarra","Categoria","FechaCaducidad","Marca","Precio","Stock","Estado"
"SUC01","Leche Entera","1234567890","Lacteos","2027-06-01","Dos Pinos",12.50,100,"Disponible"
```

> El código de barra debe tener **exactamente 10 dígitos**.

---

## Estados de un producto

| Estado | Significado |
|--------|------------|
| `Disponible` | En inventario de una sucursal, listo para venta |
| `EnTransito` | Moviéndose entre sucursales via simulación |
| `Agotado` | Stock = 0 (calculado automáticamente) |

---

## Visualización de estructuras

El Tab Visualización genera archivos `.dot` para Graphviz. Para convertirlos a PNG:

```bash
dot -Tpng output/avl.dot     -o output/avl.png
dot -Tpng output/arbolB.dot  -o output/arbolB.png
dot -Tpng output/arbolBP.dot -o output/arbolBP.png
dot -Tpng output/grafo_red.dot -o output/grafo_red.png
```

Graphviz disponible en: https://graphviz.org/download/
