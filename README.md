# Gestión de Catálogo de Productos de Supermercado — Fase 2

Curso de Estructura de Datos

---

## Descripción

Sistema de gestión de inventario **multi-sucursal** con interfaz gráfica Qt.
La red de sucursales se modela como un **grafo ponderado** con algoritmo
**Dijkstra** (optimizable por tiempo o costo). Cada sucursal administra su
propio catálogo con **6 estructuras de datos sincronizadas**, además de
**3 colas (FIFO)** y **1 pila (LIFO)** para el flujo de productos y el control
de cambios.

El sistema simula visualmente el traslado de productos entre sucursales,
respetando los tiempos de procesamiento de cada etapa (ingreso, traspaso,
despacho) y el intervalo de despacho, todo escalable por un factor de
aceleración.

---

## Estructura del proyecto

```
edd-proyecto-fase-2-gui/
├── edd-proyecto-fase-2-gui.pro   ← archivo de proyecto Qt
├── main.cpp                      ← punto de entrada
├── mainwindow.h / .cpp / .ui     ← ventana principal (9 tabs)
│
├── core/                         ← Estructuras base (Fase 1)
│   ├── include/                  ← 16 headers
│   └── src/                      ← 16 implementaciones
│       Producto, Catalogo, ListaSimple, ListaOrdenada,
│       ArbolAVL, ArbolB, ArbolBPlus, TablaHash (+ nodos),
│       CargadorCSV, MedidorRendimiento, VisualizadorDot
│
├── include/ + src/               ← Clases nuevas Fase 2
│   ├── Cola.h                    ← FIFO genérica (template, header-only)
│   ├── Pila.h                    ← LIFO genérica (template, header-only)
│   ├── Operacion.h               ← registro de historial (rollback)
│   ├── Sucursal.{h,cpp}          ← nodo de la red (catálogo + colas + pila)
│   ├── RedSucursales.{h,cpp}     ← grafo + Dijkstra + transferencias pendientes
│   ├── CargadorRed.{h,cpp}       ← carga flexible de 3 CSV con validaciones
│   └── SimuladorTransferencia.{h,cpp}  ← máquina de estados Qt (lotes animados)
│   
│
├── data/                         ← CSV de muestra (varios formatos)
├── output/                       ← archivos .dot y .png generados
└── docs/                         ← documentación del proyecto
```

---

## Compilar y ejecutar

### Requisitos
- Qt Creator 6.x con MinGW 64-bit (Windows) o GCC (Linux)
- C++17
- Graphviz (opcional, solo para convertir los `.dot` a PNG)

### En Qt Creator
1. Abrir `edd-proyecto-fase-2-gui.pro`
2. Configurar el kit (MinGW 64-bit recomendado)
3. Configurar el **Working Directory** en `$PROJECT_DIR$`
4. Tras cualquier cambio en el `.pro` o al agregar archivos: **Run qmake**
5. Compilar (`Ctrl+B`) y ejecutar (`Ctrl+R`)

> **El Working Directory es obligatorio** para que el programa encuentre la
> carpeta `data/` al cargar los CSV.

---

## Uso del sistema

Al iniciar, el sistema arranca con la **red vacía**. Se usan los botones del
**Tab Sistema** para cargar los datos.

### Tabs disponibles

| Tab | Funcionalidad |
|-----|---------------|
| **Sistema** | Carga de CSV (individual o "Cargar Todo"), estadísticas, estado de estructuras |
| **Sucursales** | CRUD de sucursales, inventario por sucursal con búsqueda en tiempo real |
| **Red** | Visualización del grafo, CRUD de conexiones (bidireccionales/unidireccionales) |
| **Inventario** | Búsqueda global con 4 criterios (AVL, Hash, B+, B), agregar/eliminar productos |
| **Transferencia** | Cálculo de ruta óptima + simulación animada **por lotes** (multi-selección) |
| **Operaciones** | Colas en vivo (FIFO) + pila de operaciones (LIFO) + devoluciones animadas |
| **Procesos (hilos)** | Transferencias pendientes agrupadas por origen→destino, procesadas por grupo |
| **Rendimiento** | Benchmark de 4 estructuras (Lista Simple, Lista Ordenada, AVL, Hash) |
| **Visualización** | Generación de `.dot` y PNG con Graphviz (AVL, B, B+, grafo) |

### Carga de datos

El Tab Sistema ofrece carga individual de cada CSV o **"Cargar Todo"** (los tres
de una vez). El orden importa: **sucursales → conexiones → productos**.

Los cargadores son **flexibles** y detectan el formato automáticamente:
- **Conexiones:** acepta 4 columnas (bidireccional por defecto) o 5 (con campo explícito)
- **Productos:** acepta 8 columnas, 9 (con Estado) o el formato **entrada/salida**

Las líneas mal formadas se omiten y se registran en `data/errors_red.log`,
sin detener la carga (rollback parcial).

---

## Flujo de colas (FIFO) y pila (LIFO)

### Las 3 colas por sucursal

Durante una transferencia, cada producto atraviesa las colas según el **rol**
de la sucursal en la ruta:

| Rol | Flujo |
|-----|-------|
| **Origen** | Ingreso → Salida → viaja (sin traspaso) |
| **Intermedia** | Ingreso → Traspaso → Salida → viaja |
| **Destino** | Ingreso → se almacena en el inventario |

Los productos se procesan **uno a uno** en cada cola, esperando el tiempo
correspondiente (ingreso, traspaso o intervalo de despacho), lo que hace
visible el comportamiento FIFO. En el Tab Operaciones, el **frente** de cada
cola (próximo a salir) se resalta en verde y el **final** (último en entrar)
en azul.

### La pila de operaciones (LIFO)

Cada operación (agregar, eliminar, transferir, devolver) se apila para permitir
**rollback**. La cima (próxima a deshacer) se resalta en naranja. El botón
"Deshacer última operación" saca de la cima, mostrando el comportamiento LIFO.

---

## Transferencias

### Por lotes (Tab Transferencia)
1. Seleccionar origen y destino
2. Marcar uno o varios productos con checkboxes (o "Lote demo")
3. Elegir criterio (tiempo/costo) y velocidad (1x–120x)
4. **Calcular ruta óptima** → muestra el camino con pesos
5. **Simular transferencia del lote** → animación con FIFO visible

### Por grupos pendientes (Tab Procesos)
Cuando un producto se carga con **sucursal de entrada distinta a la de salida**
(formato CSV entrada/salida), queda como transferencia pendiente. El Tab Procesos
las **agrupa por par origen→destino** y permite procesar un grupo a la vez,
evitando que las colas de distintas rutas se mezclen.

### Devoluciones (Tab Operaciones)
Una devolución envía el producto de vuelta a su **sucursal de entrada original**,
viajando por la red como una transferencia animada. Queda registrada en la pila
para rollback.

---

## Estructuras de datos

### Por sucursal (6 estructuras sincronizadas)

| Estructura | Clave | Complejidad búsqueda |
|-----------|-------|----------------------|
| Lista Simple | — | O(n) |
| Lista Ordenada | Nombre | O(n) con corte anticipado |
| Árbol AVL | Nombre | O(log n) |
| Árbol B (t=3) | Fecha caducidad | O(log n + k) por rango |
| Árbol B+ (t=3) | Categoría | O(log n + k) por categoría |
| Tabla Hash | Código de barra | O(1) amortizado |

> Las 6 estructuras se insertan de forma **atómica con rollback**: si una falla
> (nombre duplicado en AVL o código duplicado en Hash), se revierten todas.
> El método `Catalogo::actualizarStock()` mantiene el stock sincronizado entre
> las 6 estructuras.

### De red y flujo (Fase 2)

| Estructura | Uso | Complejidad |
|-----------|-----|-------------|
| Cola\<T\> | Flujo de productos (3 por sucursal) | O(1) todas |
| Pila\<T\> | Historial de operaciones (rollback) | O(1) todas |
| Grafo (lista de adyacencia) | Red de sucursales | O(V+E) recorrido |
| Dijkstra | Ruta óptima por tiempo/costo | O((V+E) log V) |

---

## Formato de los CSV

### sucursales.csv
```
"ID","Nombre","Ubicación","t_ingreso","t_traspaso","t_despacho"
"SUC01","Central Zona 1","Ciudad de Guatemala, Zona 1",30,45,20
```

### conexiones.csv (4 o 5 columnas)
```
"OrigenID","DestinoID","Tiempo","Costo"
"SUC01","SUC02",25,15.50
```
La 5ª columna opcional `"Bidireccional"` (true/false) permite conexiones
unidireccionales. Sin ella, todas se asumen bidireccionales.

### productos.csv (9 columnas — formato "hilos")
```
"SucursalEntradaId","SucursalSalidaId","Nombre","CodigoBarra","Categoria","FechaCaducidad","Marca","Precio","Stock"
"SUC06","SUC07","Agua Pura","1000000000","Bebidas","2026-12-04","Gallo","112.40","149"
```
Si **entrada = salida**, el producto se almacena directo en esa sucursal.
Si **entrada ≠ salida**, queda como transferencia pendiente (Tab Procesos).

> El cargador detecta el formato automáticamente leyendo el encabezado.

---

## Estados de un producto

| Estado | Significado |
|--------|------------|
| `Disponible` | En inventario de una sucursal, listo para venta |
| `EnTransito` | Moviéndose entre sucursales durante la simulación |
| `Agotado` | Stock = 0 |

---

## Visualización de estructuras

El Tab Visualización genera archivos `.dot` para Graphviz y los convierte a PNG.
Los árboles se generan **completos** (sin límite de nodos), con recorrido por
niveles (BFS) que preserva la forma real del árbol.

```bash
dot -Tpng output/avl.dot       -o output/avl.png
dot -Tpng output/arbolB.dot    -o output/arbolB.png
dot -Tpng output/arbolBP.dot   -o output/arbolBP.png
dot -Tpng output/grafo_red.dot -o output/grafo_red.png
```

En la visualización del AVL, los nodos desbalanceados (si los hubiera) se pintan
de **rojo** — en un AVL sano todos los nodos salen azules, lo que confirma
visualmente que el árbol respeta su invariante.

Graphviz disponible en: https://graphviz.org/download/
