#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QTabWidget>
#include <QFrame>
#include <QFont>
#include <QString>

/*
 * MainWindow.cpp
 * ---------------
 * Fase 0 — Pantalla de bienvenida + verificación de core/
 *
 * Al iniciar, crea un catálogo de prueba, inserta 3 productos
 * y muestra las estadísticas en pantalla. Si compila y muestra
 * los datos correctamente, core/ está bien enlazado con Qt.
 *
 * Esta pantalla se reemplazará progresivamente por los
 * módulos reales en las fases siguientes.
 */

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , catalogoPrueba(nullptr)
{
    ui->setupUi(this);
    inicializarUI();
}

MainWindow::~MainWindow()
{
    delete catalogoPrueba;
    delete ui;
}

void MainWindow::inicializarUI()
{
    // ── Crear y poblar el catálogo de prueba ──────────────────
    catalogoPrueba = new Catalogo();

    // Productos con código de barra de exactamente 10 caracteres
    catalogoPrueba->agregarProducto(
        Producto("Leche Entera", "1234567890", "Lacteos",
                 "2026-12-01", "Dos Pinos", 12.50, 100));

    catalogoPrueba->agregarProducto(
        Producto("Cafe Molido", "0987654321", "Bebidas",
                 "2027-06-15", "Cafe Rey", 35.00, 50));

    catalogoPrueba->agregarProducto(
        Producto("Arroz Blanco", "1122334455", "Granos",
                 "2028-01-01", "Tio Pelon", 18.75, 200));

    // ── Construir la UI ───────────────────────────────────────
    QWidget *central = new QWidget(this);
    setCentralWidget(central);

    QVBoxLayout *layoutPrincipal = new QVBoxLayout(central);
    layoutPrincipal->setContentsMargins(0, 0, 0, 0);
    layoutPrincipal->setSpacing(0);

    // Barra de título superior
    QFrame *barraTitulo = new QFrame();
    barraTitulo->setFixedHeight(80);
    barraTitulo->setStyleSheet(
        "background-color: #1F3864;"
        "border-bottom: 3px solid #2E75B6;");

    QHBoxLayout *layoutTitulo = new QHBoxLayout(barraTitulo);
    QLabel *titulo = new QLabel("  Gestión de Catálogo — Red de Sucursales");
    QFont fuenteTitulo("Arial", 18, QFont::Bold);
    titulo->setFont(fuenteTitulo);
    titulo->setStyleSheet("color: white;");
    layoutTitulo->addWidget(titulo);

    QLabel *version = new QLabel("v2.0  ");
    version->setStyleSheet("color: #7AADFF; font-size: 12px;");
    version->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    layoutTitulo->addWidget(version);

    layoutPrincipal->addWidget(barraTitulo);

    // Área de contenido con tabs
    QTabWidget *tabs = new QTabWidget();
    tabs->setStyleSheet(
        "QTabBar::tab { min-width: 140px; padding: 8px 16px; font-size: 13px; }"
        "QTabBar::tab:selected { background: #2E75B6; color: white; border-bottom: 3px solid #1F3864; }"
        "QTabBar::tab:!selected { background: #f0f0f0; color: #333; }"
        "QTabWidget::pane { border: 1px solid #ddd; }");

    // ── Pestaña: Bienvenida / Estado del sistema ──────────────
    QWidget *tabBienvenida = new QWidget();
    construirPantallaBienvenida(tabBienvenida);
    tabs->addTab(tabBienvenida, "  Sistema  ");

    // ── Pestañas placeholder (se implementan en fases siguientes)
    auto placeholder = [](const QString &msg) {
        QWidget *w = new QWidget();
        QVBoxLayout *l = new QVBoxLayout(w);
        QLabel *lbl = new QLabel(msg);
        lbl->setAlignment(Qt::AlignCenter);
        lbl->setStyleSheet("color: #888; font-size: 14px;");
        l->addWidget(lbl);
        return w;
    };

    tabs->addTab(placeholder("Módulo de Sucursales\n(Fase 2 — próximamente)"),
                 "  Sucursales  ");
    tabs->addTab(placeholder("Red de Sucursales — Grafo\n(Fase 2 — próximamente)"),
                 "  Red  ");
    tabs->addTab(placeholder("Gestión de Inventario\n(Fase 2 — próximamente)"),
                 "  Inventario  ");
    tabs->addTab(placeholder("Transferencia entre Sucursales\n(Fase 3 — próximamente)"),
                 "  Transferencia  ");
    tabs->addTab(placeholder("Medición de Rendimiento\n(Fase 6 — próximamente)"),
                 "  Rendimiento  ");

    layoutPrincipal->addWidget(tabs);

    // Barra de estado
    statusBar()->showMessage(
        QString("  Sistema iniciado  |  Productos en catálogo de prueba: %1  |"
                "  Estructuras activas: AVL · B · B+ · Hash · Lista Simple · Lista Ordenada")
            .arg(catalogoPrueba->contarProductos()));
    statusBar()->setStyleSheet("background: #1F3864; color: #AAD4FF; font-size: 11px;");
}

void MainWindow::construirPantallaBienvenida(QWidget *contenedor)
{
    QVBoxLayout *layout = new QVBoxLayout(contenedor);
    layout->setContentsMargins(40, 40, 40, 40);
    layout->setSpacing(20);

    // Título de sección
    QLabel *secTitulo = new QLabel("Estado del Sistema");
    QFont fuenteSec("Arial", 16, QFont::Bold);
    secTitulo->setFont(fuenteSec);
    secTitulo->setStyleSheet("color: #1F3864; border-bottom: 2px solid #2E75B6;"
                             "padding-bottom: 8px;");
    layout->addWidget(secTitulo);

    // ── Verificación de core/ ─────────────────────────────────
    bool avlOk   = catalogoPrueba->obtenerArbolAVL()->obtenerTamano()   == 3;
    bool hashOk  = catalogoPrueba->obtenerTablaHash()->obtenerTamano()  == 3;
    bool bOk     = catalogoPrueba->obtenerArbolB()->obtenerTamano()     == 3;
    bool bplusOk = catalogoPrueba->obtenerArbolBPlus()->obtenerTamano() == 3;
    bool lsOk    = catalogoPrueba->obtenerListaSimple()->obtenerTamano()   == 3;
    bool loOk    = catalogoPrueba->obtenerListaOrdenada()->obtenerTamano() == 3;

    auto iconoEstado = [](bool ok) -> QString {
        return ok ? "✔" : "✘";
    };
    auto colorEstado = [](bool ok) -> QString {
        return ok ? "color: #1E6B2E; font-weight: bold;"
                  : "color: #8B0000; font-weight: bold;";
    };

    // Tarjeta de estructuras
    QFrame *tarjetaEstructuras = new QFrame();
    tarjetaEstructuras->setStyleSheet(
        "background: #F8FBFF; border: 1px solid #C5D8F0;"
        "border-radius: 6px; padding: 8px;");
    QVBoxLayout *layoutTarjeta = new QVBoxLayout(tarjetaEstructuras);

    QLabel *tarjetaTitulo = new QLabel("Verificación de estructuras core/");
    tarjetaTitulo->setStyleSheet("font-size: 13px; font-weight: bold; color: #2E75B6;");
    layoutTarjeta->addWidget(tarjetaTitulo);

    struct { QString nombre; bool ok; } estructuras[] = {
                       { "Lista Simple   (O(1) inserción, O(n) búsqueda)",   lsOk   },
                       { "Lista Ordenada (O(n) inserción, O(n) búsqueda*)",  loOk   },
                       { "Árbol AVL      (O(log n) búsqueda por nombre)",    avlOk  },
                       { "Árbol B        (O(log n+k) rango de fechas)",      bOk    },
                       { "Árbol B+       (O(log n+k) búsqueda por categ.)", bplusOk},
                       { "Tabla Hash     (O(1) búsqueda por código barra)",  hashOk },
                       };

    for (auto &e : estructuras) {
        QHBoxLayout *fila = new QHBoxLayout();
        QLabel *icono = new QLabel(iconoEstado(e.ok) + "  ");
        icono->setStyleSheet(colorEstado(e.ok) + " font-size: 14px;");
        icono->setFixedWidth(28);
        QLabel *nombre = new QLabel(e.nombre);
        nombre->setStyleSheet("font-size: 12px; font-family: Courier New;");
        fila->addWidget(icono);
        fila->addWidget(nombre);
        fila->addStretch();
        layoutTarjeta->addLayout(fila);
    }

    layout->addWidget(tarjetaEstructuras);

    // ── Resumen del catálogo de prueba ────────────────────────
    QFrame *tarjetaResumen = new QFrame();
    tarjetaResumen->setStyleSheet(
        "background: #F8FFF8; border: 1px solid #B0D8B0;"
        "border-radius: 6px; padding: 8px;");
    QVBoxLayout *layoutResumen = new QVBoxLayout(tarjetaResumen);

    QLabel *resumenTitulo = new QLabel("Catálogo de prueba (3 productos insertados)");
    resumenTitulo->setStyleSheet("font-size: 13px; font-weight: bold; color: #1E6B2E;");
    layoutResumen->addWidget(resumenTitulo);

    // Buscar por código (verifica la Hash en acción)
    Producto *p = catalogoPrueba->buscarPorCodigo("1234567890");
    QString infoBusqueda = p
                               ? QString("✔  buscarPorCodigo(\"1234567890\") → encontrado: %1").arg(
                                     QString::fromStdString(p->nombre))
                               : "✘  buscarPorCodigo falló";

    QLabel *lblBusqueda = new QLabel(infoBusqueda);
    lblBusqueda->setStyleSheet(p
                                   ? "font-size: 12px; color: #1E6B2E; font-family: Courier New;"
                                   : "font-size: 12px; color: #8B0000; font-family: Courier New;");
    layoutResumen->addWidget(lblBusqueda);

    // Buscar por nombre (verifica el AVL)
    Producto *p2 = catalogoPrueba->buscarPorNombre("Cafe Molido");
    QString infoBusqueda2 = p2
                                ? QString("✔  buscarPorNombre(\"Cafe Molido\")   → encontrado: %1  Q%2")
                                      .arg(QString::fromStdString(p2->nombre))
                                      .arg(p2->precio, 0, 'f', 2)
                                : "✘  buscarPorNombre falló";

    QLabel *lblBusqueda2 = new QLabel(infoBusqueda2);
    lblBusqueda2->setStyleSheet(p2
                                    ? "font-size: 12px; color: #1E6B2E; font-family: Courier New;"
                                    : "font-size: 12px; color: #8B0000; font-family: Courier New;");
    layoutResumen->addWidget(lblBusqueda2);

    layout->addWidget(tarjetaResumen);

    // ── Próximos pasos ────────────────────────────────────────
    QFrame *tarjetaProximos = new QFrame();
    tarjetaProximos->setStyleSheet(
        "background: #FFFDF0; border: 1px solid #D4C56A;"
        "border-radius: 6px; padding: 8px;");
    QVBoxLayout *layoutProximos = new QVBoxLayout(tarjetaProximos);

    QLabel *proximosTitulo = new QLabel("Plan de implementación");
    proximosTitulo->setStyleSheet("font-size: 13px; font-weight: bold; color: #7A6000;");
    layoutProximos->addWidget(proximosTitulo);

    QStringList pasos = {
        "✔  Fase 0 — Migración y verificación de core/         [COMPLETADA]",
        "○  Fase 1 — Sucursal, Cola, Pila, RedSucursales       [siguiente]",
        "○  Fase 2 — Dijkstra y simulación de despacho",
        "○  Fase 3 — CSV multi-entidad (sucursales, conexiones)",
        "○  Fase 4 — Interfaz completa por módulos",
        "○  Fase 5 — Visualización de grafos y estructuras",
        "○  Fase 6 — Documentación y entregables",
    };

    for (const QString &paso : pasos) {
        QLabel *lbl = new QLabel("  " + paso);
        bool completado = paso.startsWith("✔");
        lbl->setStyleSheet(completado
                               ? "font-size: 12px; color: #1E6B2E; font-family: Courier New;"
                               : "font-size: 12px; color: #555; font-family: Courier New;");
        layoutProximos->addWidget(lbl);
    }

    layout->addWidget(tarjetaProximos);
    layout->addStretch();
}
