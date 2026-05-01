#include "VisualizadorDot.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QFormLayout>
#include <QSplitter>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QComboBox>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QHeaderView>
#include <QGroupBox>
#include <QFrame>
#include <QTextEdit>
#include <QListWidget>
#include <QMessageBox>
#include <QInputDialog>
#include <QDialog>
#include <QDialogButtonBox>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsEllipseItem>
#include <QGraphicsLineItem>
#include <QGraphicsTextItem>
#include <QScrollArea>
#include <QProgressBar>
#include <QString>
#include <QFont>
#include <QColor>
#include <QPalette>
#include <QTimer>
#include <QProcess>
#include <QFileDialog>
#include <QPixmap>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QRadioButton>
#include <QButtonGroup>
#include <QCheckBox>
#include <cmath>

// ── Colores y estilos del sistema ─────────────────────────────
static const QString AZUL_OSCURO  = "#1F3864";
static const QString AZUL_MEDIO   = "#2E75B6";
static const QString AZUL_CLARO   = "#BDD7EE";
static const QString VERDE        = "#1E6B2E";
static const QString VERDE_CLARO  = "#E8F5E9";
static const QString ROJO         = "#8B0000";
static const QString ROJO_CLARO   = "#FFEBEE";
static const QString GRIS_CLARO   = "#F5F5F5";
static const QString GRIS_BORDE   = "#DDDDDD";
static const QString AMARILLO     = "#FFF9C4";
static const QString NARANJA      = "#E65100";

QString MainWindow::estiloBoton(const QString &color) {
    return QString(
               "QPushButton {"
               "  background-color: %1; color: white; border: none;"
               "  border-radius: 4px; padding: 7px 16px; font-size: 12px; font-weight: bold;"
               "}"
               "QPushButton:hover { background-color: %1; opacity: 0.85; border: 1px solid white; }"
               "QPushButton:pressed { padding: 8px 15px 6px 17px; }"
               ).arg(color);
}

QString MainWindow::estiloHeader() {
    return QString(
               "background-color: %1; color: white;"
               "padding: 6px 10px; font-weight: bold; font-size: 12px;"
               ).arg(AZUL_OSCURO);
}

// ── Constructor / Destructor ──────────────────────────────────

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , red(new RedSucursales())
{
    ui->setupUi(this);
    setWindowTitle("Gestión de Catálogo — Red de Sucursales");
    resize(1280, 820);
    setMinimumSize(1100, 700);
    cargarDatos();
    construirUI();
}

MainWindow::~MainWindow() {
    delete red;
    delete ui;
}

// ── Carga inicial de datos ────────────────────────────────────

void MainWindow::cargarDatos() {
    CargadorRed cargador("data/errors_red.log");
    cargador.cargarTodo(
        "data/sucursales.csv",
        "data/conexiones.csv",
        "data/productos_fase2.csv",
        *red);
}

// ── Construcción de la UI principal ──────────────────────────

void MainWindow::construirUI() {
    QWidget *central = new QWidget(this);
    setCentralWidget(central);
    QVBoxLayout *mainLayout = new QVBoxLayout(central);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // Barra de título
    QFrame *header = new QFrame();
    header->setFixedHeight(52);
    header->setStyleSheet(
        QString("background-color: %1; border-bottom: 3px solid %2;")
            .arg(AZUL_OSCURO).arg(AZUL_MEDIO));
    QHBoxLayout *hl = new QHBoxLayout(header);
    hl->setContentsMargins(16, 0, 16, 0);

    QLabel *titulo = new QLabel("  Gestión de Catálogo — Red de Sucursales");
    titulo->setStyleSheet("color: white; font-size: 16px; font-weight: bold;");
    hl->addWidget(titulo);
    hl->addStretch();

    int totalProd = 0;
    for (auto s : red->obtenerSucursales()) totalProd += s->contarProductos();
    QLabel *stats = new QLabel(
        QString("  %1 sucursales  |  %2 conexiones  |  %3 productos  ")
            .arg(red->contarSucursales())
            .arg(red->contarConexiones())
            .arg(totalProd));
    stats->setStyleSheet("color: #AAD4FF; font-size: 12px;");
    hl->addWidget(stats);
    mainLayout->addWidget(header);

    // Tabs
    tabs = new QTabWidget();
    tabs->setStyleSheet(
        "QTabBar::tab { min-width: 120px; padding: 9px 16px; font-size: 12px; font-weight: bold; }"
        "QTabBar::tab:selected { background: " + AZUL_MEDIO + "; color: white; }"
                       "QTabBar::tab:!selected { background: #EEEEEE; color: #444; }"
                       "QTabWidget::pane { border: 1px solid " + GRIS_BORDE + "; }");

    tabs->addTab(crearTabSistema(),       "  Sistema  ");
    tabs->addTab(crearTabSucursales(),    "  Sucursales  ");
    tabs->addTab(crearTabRed(),           "  Red  ");
    tabs->addTab(crearTabInventario(),    "  Inventario  ");
    tabs->addTab(crearTabTransferencia(), "  Transferencia  ");
    tabs->addTab(crearTabRendimiento(),   "  Rendimiento  ");
    tabs->addTab(crearTabEstructuras(),   "  Visualización  ");

    connect(tabs, &QTabWidget::currentChanged, this, &MainWindow::onTabCambiado);
    mainLayout->addWidget(tabs);

    // Barra de estado
    lblEstado = new QLabel();
    actualizarBarraEstado();
    statusBar()->addWidget(lblEstado);
    statusBar()->setStyleSheet(
        "background: " + AZUL_OSCURO + "; color: #AAD4FF; font-size: 11px;");

    connect(this, &MainWindow::redActualizada, this, &MainWindow::onRedActualizada);
}

void MainWindow::actualizarBarraEstado() {
    int total = 0;
    for (auto s : red->obtenerSucursales()) total += s->contarProductos();
    lblEstado->setText(
        QString("  Red activa: %1 sucursales  |  %2 conexiones  |"
                "  %3 productos totales  |  6 estructuras por sucursal  ")
            .arg(red->contarSucursales())
            .arg(red->contarConexiones())
            .arg(total));
}

void MainWindow::onRedActualizada() { actualizarBarraEstado(); }
void MainWindow::onTabCambiado(int) { actualizarBarraEstado(); }

// ════════════════════════════════════════════════════════════
// TAB 1 — SISTEMA
// ════════════════════════════════════════════════════════════
QWidget* MainWindow::crearTabSistema() {
    QWidget *w = new QWidget();
    QVBoxLayout *lay = new QVBoxLayout(w);
    lay->setContentsMargins(24, 20, 24, 20);
    lay->setSpacing(16);

    // Título
    QLabel *titulo = new QLabel("Estado del Sistema");
    titulo->setStyleSheet(
        QString("color: %1; font-size: 18px; font-weight: bold;"
                "border-bottom: 2px solid %2; padding-bottom: 8px;")
            .arg(AZUL_OSCURO).arg(AZUL_MEDIO));
    lay->addWidget(titulo);

    // Grid de tarjetas de estadísticas
    QHBoxLayout *cards = new QHBoxLayout();
    cards->setSpacing(12);

    auto tarjeta = [&](const QString &valor, const QString &etiqueta,
                       const QString &color) {
        QFrame *f = new QFrame();
        f->setStyleSheet(QString(
                             "background: %1; border-radius: 8px;"
                             "border: 1px solid %2;").arg(color).arg(GRIS_BORDE));
        f->setFixedHeight(90);
        QVBoxLayout *fl = new QVBoxLayout(f);
        fl->setAlignment(Qt::AlignCenter);
        QLabel *lv = new QLabel(valor);
        lv->setAlignment(Qt::AlignCenter);
        lv->setStyleSheet(
            QString("color: %1; font-size: 26px; font-weight: bold;").arg(AZUL_OSCURO));
        QLabel *le = new QLabel(etiqueta);
        le->setAlignment(Qt::AlignCenter);
        le->setStyleSheet("color: #555; font-size: 11px;");
        fl->addWidget(lv); fl->addWidget(le);
        return f;
    };

    int totalProd = 0;
    for (auto s : red->obtenerSucursales()) totalProd += s->contarProductos();

    cards->addWidget(tarjeta(QString::number(red->contarSucursales()),
                             "Sucursales", AZUL_CLARO));
    cards->addWidget(tarjeta(QString::number(red->contarConexiones()),
                             "Conexiones en la red", AZUL_CLARO));
    cards->addWidget(tarjeta(QString::number(totalProd),
                             "Productos totales", VERDE_CLARO));
    cards->addWidget(tarjeta("6",
                             "Estructuras por sucursal", AMARILLO));
    lay->addLayout(cards);

    // Estado de estructuras por sucursal
    QGroupBox *gbEst = new QGroupBox("Estructuras de datos activas por sucursal");
    gbEst->setStyleSheet(
        "QGroupBox { font-weight: bold; font-size: 13px; color: " + AZUL_OSCURO + "; "
                                                                                  "border: 1px solid " + GRIS_BORDE + "; border-radius: 6px; margin-top: 8px; }"
                       "QGroupBox::title { subcontrol-origin: margin; left: 12px; padding: 0 4px; }");
    QGridLayout *glay = new QGridLayout(gbEst);

    struct { QString nombre; QString complejidad; QString descripcion; } estructuras[] = {
                       {"Lista Simple",   "O(1) ins / O(n) búsq",  "Orden de inserción, base comparativa"},
                       {"Lista Ordenada", "O(n) ins / O(n) búsq*", "Orden alfabético, corte anticipado"},
                       {"Árbol AVL",      "O(log n)",               "Búsqueda por nombre, balance garantizado"},
                       {"Árbol B",        "O(log n+k)",             "Búsqueda por rango de fechas"},
                       {"Árbol B+",       "O(log n+k)",             "Búsqueda por categoría, hojas enlazadas"},
                       {"Tabla Hash",     "O(1) amortizado",        "Búsqueda por código de barra (10 dígitos)"},
                       };

    QStringList colores = {AZUL_CLARO, AZUL_CLARO, VERDE_CLARO,
                           "#FFE0B2", "#FFE0B2", AMARILLO};
    for (int i = 0; i < 6; i++) {
        QFrame *ef = new QFrame();
        ef->setStyleSheet(QString(
                              "background: %1; border-radius: 4px; padding: 4px;").arg(colores[i]));
        QHBoxLayout *el = new QHBoxLayout(ef);
        el->setContentsMargins(8, 4, 8, 4);
        QLabel *check = new QLabel("✔");
        check->setStyleSheet("color: " + VERDE + "; font-size: 14px; font-weight: bold;");
        check->setFixedWidth(20);
        QLabel *nombre = new QLabel(
            QString("<b>%1</b>  <span style='color:#555; font-size:11px;'>%2</span>")
                .arg(estructuras[i].nombre).arg(estructuras[i].descripcion));
        nombre->setTextFormat(Qt::RichText);
        QLabel *comp = new QLabel(estructuras[i].complejidad);
        comp->setStyleSheet(
            "color: " + AZUL_OSCURO + "; font-family: Courier New;"
                                      " font-size: 11px; font-weight: bold;");
        comp->setFixedWidth(160);
        comp->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
        el->addWidget(check); el->addWidget(nombre); el->addStretch(); el->addWidget(comp);
        glay->addWidget(ef, i / 2, i % 2);
    }
    lay->addWidget(gbEst);

    // Resumen por sucursal
    QGroupBox *gbSuc = new QGroupBox("Inventario por sucursal");
    gbSuc->setStyleSheet(gbEst->styleSheet());
    QVBoxLayout *sucLay = new QVBoxLayout(gbSuc);

    QTableWidget *tbl = new QTableWidget();
    tbl->setColumnCount(5);
    tbl->setHorizontalHeaderLabels({"ID", "Nombre", "Ubicación",
                                    "Productos", "Tiempos (ing/tras/des)"});
    tbl->horizontalHeader()->setStretchLastSection(true);
    tbl->horizontalHeader()->setStyleSheet(estiloHeader());
    tbl->setSelectionBehavior(QAbstractItemView::SelectRows);
    tbl->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tbl->setAlternatingRowColors(true);
    tbl->setStyleSheet("alternate-background-color: " + GRIS_CLARO + ";");
    tbl->verticalHeader()->setVisible(false);

    auto sucursales = red->obtenerSucursales();
    tbl->setRowCount((int)sucursales.size());
    for (int i = 0; i < (int)sucursales.size(); i++) {
        Sucursal *s = sucursales[i];
        tbl->setItem(i, 0, new QTableWidgetItem(QString::fromStdString(s->getId())));
        tbl->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(s->getNombre())));
        tbl->setItem(i, 2, new QTableWidgetItem(QString::fromStdString(s->getUbicacion())));
        tbl->setItem(i, 3, new QTableWidgetItem(QString::number(s->contarProductos())));
        tbl->setItem(i, 4, new QTableWidgetItem(
                               QString("%1s / %2s / %3s")
                                   .arg(s->getTiempoIngreso())
                                   .arg(s->getTiempoTraspaso())
                                   .arg(s->getTiempoDespacho())));
    }
    tbl->resizeColumnsToContents();
    sucLay->addWidget(tbl);
    lay->addWidget(gbSuc);
    lay->addStretch();
    return w;
}

// ════════════════════════════════════════════════════════════
// TAB 2 — SUCURSALES
// ════════════════════════════════════════════════════════════
QWidget* MainWindow::crearTabSucursales() {
    QWidget *w = new QWidget();
    QHBoxLayout *lay = new QHBoxLayout(w);
    lay->setContentsMargins(12, 12, 12, 12);
    lay->setSpacing(12);

    // Panel izquierdo — lista de sucursales
    QGroupBox *gbLista = new QGroupBox("Sucursales en la red");
    gbLista->setStyleSheet(
        "QGroupBox { font-weight: bold; color: " + AZUL_OSCURO + ";"
                                                                 " border: 1px solid " + GRIS_BORDE + "; border-radius: 6px; margin-top: 8px; }"
                       "QGroupBox::title { subcontrol-origin: margin; left: 12px; padding: 0 4px; }");
    gbLista->setFixedWidth(280);
    QVBoxLayout *lLay = new QVBoxLayout(gbLista);

    QListWidget *listaSuc = new QListWidget();
    listaSuc->setStyleSheet(
        "QListWidget::item { padding: 10px 8px; border-bottom: 1px solid " + GRIS_BORDE + "; }"
                                                                                          "QListWidget::item:selected { background: " + AZUL_MEDIO + "; color: white; }");
    for (Sucursal *s : red->obtenerSucursales()) {
        listaSuc->addItem(
            QString("[%1] %2")
                .arg(QString::fromStdString(s->getId()))
                .arg(QString::fromStdString(s->getNombre())));
    }
    lLay->addWidget(listaSuc);

    QHBoxLayout *btnLay = new QHBoxLayout();
    QPushButton *btnAgregar  = new QPushButton("+ Agregar");
    QPushButton *btnEliminar = new QPushButton("✕ Eliminar");
    btnAgregar->setStyleSheet(estiloBoton(VERDE));
    btnEliminar->setStyleSheet(estiloBoton(ROJO));
    btnLay->addWidget(btnAgregar);
    btnLay->addWidget(btnEliminar);
    lLay->addLayout(btnLay);
    lay->addWidget(gbLista);

    // Panel derecho — detalle de la sucursal seleccionada
    QSplitter *splitter = new QSplitter(Qt::Vertical);

    // Info de la sucursal
    QGroupBox *gbInfo = new QGroupBox("Detalle de sucursal");
    gbInfo->setStyleSheet(gbLista->styleSheet());
    QFormLayout *fLay = new QFormLayout(gbInfo);
    fLay->setSpacing(10);
    fLay->setContentsMargins(16, 16, 16, 16);

    auto campo = [](const QString &v = "") {
        QLineEdit *e = new QLineEdit(v);
        e->setStyleSheet(
            "border: 1px solid " + GRIS_BORDE + "; border-radius: 4px;"
                                                " padding: 5px 8px; font-size: 12px;");
        return e;
    };
    auto campoSpin = []() {
        QSpinBox *s = new QSpinBox();
        s->setRange(0, 9999); s->setSuffix(" s");
        s->setStyleSheet(
            "border: 1px solid " + GRIS_BORDE + "; border-radius: 4px;"
                                                " padding: 4px; font-size: 12px;");
        return s;
    };

    QLineEdit *eId        = campo(); eId->setReadOnly(true);
    eId->setStyleSheet(eId->styleSheet() + " background: " + GRIS_CLARO + ";");
    QLineEdit *eNombre    = campo();
    QLineEdit *eUbicacion = campo();
    QSpinBox  *eTIngreso  = campoSpin();
    QSpinBox  *eTTraspaso = campoSpin();
    QSpinBox  *eTDespacho = campoSpin();

    fLay->addRow("ID:",                eId);
    fLay->addRow("Nombre:",            eNombre);
    fLay->addRow("Ubicación:",         eUbicacion);
    fLay->addRow("T. Ingreso (s):",    eTIngreso);
    fLay->addRow("T. Traspaso (s):",   eTTraspaso);
    fLay->addRow("T. Despacho (s):",   eTDespacho);

    QPushButton *btnGuardar = new QPushButton("  Guardar cambios");
    btnGuardar->setStyleSheet(estiloBoton(AZUL_MEDIO));
    btnGuardar->setEnabled(false);
    fLay->addRow("", btnGuardar);
    splitter->addWidget(gbInfo);

    // Inventario de la sucursal seleccionada
    QGroupBox *gbInv = new QGroupBox("Inventario de la sucursal");
    gbInv->setStyleSheet(gbLista->styleSheet());
    QVBoxLayout *invLay = new QVBoxLayout(gbInv);

    QLineEdit *buscarInv = new QLineEdit();
    buscarInv->setPlaceholderText("Buscar producto en esta sucursal...");
    buscarInv->setStyleSheet(campo()->styleSheet());
    invLay->addWidget(buscarInv);

    QTableWidget *tblInv = new QTableWidget();
    tblInv->setColumnCount(6);
    tblInv->setHorizontalHeaderLabels(
        {"Nombre","Código","Categoría","Caducidad","Precio Q","Stock"});
    tblInv->horizontalHeader()->setStretchLastSection(true);
    tblInv->horizontalHeader()->setStyleSheet(estiloHeader());
    tblInv->setSelectionBehavior(QAbstractItemView::SelectRows);
    tblInv->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tblInv->setAlternatingRowColors(true);
    tblInv->verticalHeader()->setVisible(false);
    invLay->addWidget(tblInv);
    splitter->addWidget(gbInv);
    splitter->setSizes({220, 380});
    lay->addWidget(splitter, 1);

    // ── Conexiones de señales ─────────────────────────────────

    // Al seleccionar una sucursal, rellenar los campos
    auto cargarSucursal = [=](int row) {
        if (row < 0) return;
        auto sucursales = red->obtenerSucursales();
        if (row >= (int)sucursales.size()) return;
        Sucursal *s = sucursales[row];

        eId->setText(QString::fromStdString(s->getId()));
        eNombre->setText(QString::fromStdString(s->getNombre()));
        eUbicacion->setText(QString::fromStdString(s->getUbicacion()));
        eTIngreso->setValue(s->getTiempoIngreso());
        eTTraspaso->setValue(s->getTiempoTraspaso());
        eTDespacho->setValue(s->getTiempoDespacho());
        btnGuardar->setEnabled(true);
        gbInfo->setTitle("Detalle — " + QString::fromStdString(s->getNombre()));

        // Cargar inventario
        Catalogo *cat = s->getCatalogo();
        // Usamos la lista ordenada para mostrar en A-Z
        // Construimos la tabla — recorremos via AVL in-order capturando en vector
        struct Collector {
            static void recolectar(NodoAVL *n, QTableWidget *t, int &r) {
                if (!n) return;
                recolectar(n->izquierda, t, r);
                Producto &p = n->dato;
                t->setRowCount(r + 1);
                t->setItem(r, 0, new QTableWidgetItem(QString::fromStdString(p.nombre)));
                t->setItem(r, 1, new QTableWidgetItem(QString::fromStdString(p.codigoBarra)));
                t->setItem(r, 2, new QTableWidgetItem(QString::fromStdString(p.categoria)));
                t->setItem(r, 3, new QTableWidgetItem(QString::fromStdString(p.fechaCaducidad)));
                t->setItem(r, 4, new QTableWidgetItem(QString("Q%1").arg(p.precio, 0, 'f', 2)));
                t->setItem(r, 5, new QTableWidgetItem(QString::number(p.stock)));
                r++;
                recolectar(n->derecha, t, r);
            }
        };
        tblInv->setRowCount(0);
        int fila = 0;
        Collector::recolectar(cat->obtenerArbolAVL()->obtenerRaiz(), tblInv, fila);
        tblInv->resizeColumnsToContents();
        gbInv->setTitle(QString("Inventario — %1  (%2 productos)")
                            .arg(QString::fromStdString(s->getNombre()))
                            .arg(s->contarProductos()));
    };

    connect(listaSuc, &QListWidget::currentRowChanged, cargarSucursal);

    // Buscar en inventario
    connect(buscarInv, &QLineEdit::textChanged, [=](const QString &texto) {
        for (int i = 0; i < tblInv->rowCount(); i++) {
            bool visible = texto.isEmpty() ||
                           tblInv->item(i, 0)->text().contains(texto, Qt::CaseInsensitive) ||
                           tblInv->item(i, 1)->text().contains(texto, Qt::CaseInsensitive) ||
                           tblInv->item(i, 2)->text().contains(texto, Qt::CaseInsensitive);
            tblInv->setRowHidden(i, !visible);
        }
    });

    // Guardar cambios de la sucursal
    connect(btnGuardar, &QPushButton::clicked, [=]() {
        QString id = eId->text();
        Sucursal *s = red->buscarSucursal(id.toStdString());
        if (!s) return;
        s->setNombre(eNombre->text().toStdString());
        s->setUbicacion(eUbicacion->text().toStdString());
        s->setTiempoIngreso(eTIngreso->value());
        s->setTiempoTraspaso(eTTraspaso->value());
        s->setTiempoDespacho(eTDespacho->value());

        // Actualizar la lista
        int row = listaSuc->currentRow();
        listaSuc->item(row)->setText(
            QString("[%1] %2").arg(id).arg(eNombre->text()));
        QMessageBox::information(w, "Guardado",
                                 "Sucursal actualizada correctamente.");
        emit redActualizada();
    });

    // Agregar sucursal
    connect(btnAgregar, &QPushButton::clicked, [=]() {
        QDialog dlg(w);
        dlg.setWindowTitle("Nueva Sucursal");
        dlg.setFixedWidth(380);
        QFormLayout *fl = new QFormLayout(&dlg);
        fl->setContentsMargins(20, 20, 20, 20);
        fl->setSpacing(10);

        QLineEdit *dId  = new QLineEdit(); dId->setMaxLength(10);
        QLineEdit *dNom = new QLineEdit();
        QLineEdit *dUbi = new QLineEdit();
        QSpinBox  *dTI  = new QSpinBox(); dTI->setRange(0,9999); dTI->setValue(30); dTI->setSuffix(" s");
        QSpinBox  *dTT  = new QSpinBox(); dTT->setRange(0,9999); dTT->setValue(45); dTT->setSuffix(" s");
        QSpinBox  *dTD  = new QSpinBox(); dTD->setRange(0,9999); dTD->setValue(20); dTD->setSuffix(" s");

        fl->addRow("ID (ej: SUC08):", dId);
        fl->addRow("Nombre:",         dNom);
        fl->addRow("Ubicación:",      dUbi);
        fl->addRow("T. Ingreso:",     dTI);
        fl->addRow("T. Traspaso:",    dTT);
        fl->addRow("T. Despacho:",    dTD);

        QDialogButtonBox *bb = new QDialogButtonBox(
            QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
        fl->addRow(bb);
        connect(bb, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
        connect(bb, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);

        if (dlg.exec() == QDialog::Accepted) {
            if (dId->text().isEmpty() || dNom->text().isEmpty()) {
                QMessageBox::warning(w, "Error", "ID y Nombre son obligatorios.");
                return;
            }
            Sucursal *ns = new Sucursal(
                dId->text().toStdString(),
                dNom->text().toStdString(),
                dUbi->text().toStdString(),
                dTI->value(), dTT->value(), dTD->value());
            if (red->agregarSucursal(ns)) {
                listaSuc->addItem(
                    QString("[%1] %2").arg(dId->text()).arg(dNom->text()));
                emit redActualizada();
            } else {
                delete ns;
                QMessageBox::warning(w, "Error", "Ya existe una sucursal con ese ID.");
            }
        }
    });

    // Eliminar sucursal
    connect(btnEliminar, &QPushButton::clicked, [=]() {
        int row = listaSuc->currentRow();
        if (row < 0) { QMessageBox::information(w, "Info", "Selecciona una sucursal."); return; }
        auto suc = red->obtenerSucursales()[row];
        int r = QMessageBox::question(w, "Confirmar",
                                      QString("¿Eliminar la sucursal '%1'?\nEsto eliminará también su inventario.")
                                          .arg(QString::fromStdString(suc->getNombre())));
        if (r == QMessageBox::Yes) {
            QString id = QString::fromStdString(suc->getId());
            red->eliminarSucursal(id.toStdString());
            delete listaSuc->takeItem(row);
            tblInv->setRowCount(0);
            eId->clear(); eNombre->clear(); eUbicacion->clear();
            btnGuardar->setEnabled(false);
            emit redActualizada();
        }
    });

    // Seleccionar primera sucursal al iniciar
    if (listaSuc->count() > 0) listaSuc->setCurrentRow(0);
    return w;
}

// ════════════════════════════════════════════════════════════
// TAB 3 — RED (GRAFO)
// ════════════════════════════════════════════════════════════
QWidget* MainWindow::crearTabRed() {
    QWidget *w = new QWidget();
    QHBoxLayout *lay = new QHBoxLayout(w);
    lay->setContentsMargins(12, 12, 12, 12);
    lay->setSpacing(12);

    // Panel de controles izquierdo
    QWidget *panel = new QWidget();
    panel->setFixedWidth(260);
    QVBoxLayout *pLay = new QVBoxLayout(panel);
    pLay->setSpacing(10);

    QGroupBox *gbCtrl = new QGroupBox("Controles");
    gbCtrl->setStyleSheet(
        "QGroupBox { font-weight: bold; color: " + AZUL_OSCURO + ";"
                                                                 " border: 1px solid " + GRIS_BORDE + "; border-radius: 6px; margin-top: 8px; }"
                       "QGroupBox::title { subcontrol-origin: margin; left: 12px; padding: 0 4px; }");
    QVBoxLayout *cLay = new QVBoxLayout(gbCtrl);

    QPushButton *btnRedibujar = new QPushButton("↺  Redibujar grafo");
    QPushButton *btnAgregarConn = new QPushButton("+  Nueva conexión");
    QPushButton *btnElimConn  = new QPushButton("✕  Eliminar conexión");
    btnRedibujar->setStyleSheet(estiloBoton(AZUL_MEDIO));
    btnAgregarConn->setStyleSheet(estiloBoton(VERDE));
    btnElimConn->setStyleSheet(estiloBoton(ROJO));
    cLay->addWidget(btnRedibujar);
    cLay->addWidget(btnAgregarConn);
    cLay->addWidget(btnElimConn);
    pLay->addWidget(gbCtrl);

    // Lista de conexiones
    QGroupBox *gbConns = new QGroupBox("Conexiones");
    gbConns->setStyleSheet(gbCtrl->styleSheet());
    QVBoxLayout *connLay = new QVBoxLayout(gbConns);
    QListWidget *listaConns = new QListWidget();
    listaConns->setStyleSheet(
        "QListWidget::item { padding: 6px 8px; font-size: 11px;"
        " border-bottom: 1px solid " + GRIS_BORDE + "; }"
                       "QListWidget::item:selected { background: " + AZUL_CLARO + "; }");
    for (const Conexion &c : red->obtenerConexiones()) {
        listaConns->addItem(
            QString("%1 → %2  t=%3s  Q%4")
                .arg(QString::fromStdString(c.origenId))
                .arg(QString::fromStdString(c.destinoId))
                .arg(c.tiempo).arg(c.costo));
    }
    connLay->addWidget(listaConns);
    pLay->addWidget(gbConns, 1);
    lay->addWidget(panel);

    // Vista del grafo con QGraphicsScene
    QGraphicsScene *scene = new QGraphicsScene();
    scene->setBackgroundBrush(Qt::white);
    QGraphicsView *view = new QGraphicsView(scene);
    view->setRenderHint(QPainter::Antialiasing);
    view->setDragMode(QGraphicsView::ScrollHandDrag);
    view->setStyleSheet("border: 1px solid " + GRIS_BORDE + ";");

    auto dibujarGrafo = [=]() {
        scene->clear();
        auto sucursales = red->obtenerSucursales();
        int n = (int)sucursales.size();
        if (n == 0) return;

        // Posicionar nodos en círculo
        const double R = 260.0;
        const double cx = 320.0, cy = 280.0;
        QMap<QString, QPointF> posiciones;

        for (int i = 0; i < n; i++) {
            double ang = 2.0 * M_PI * i / n - M_PI / 2.0;
            double x = cx + R * std::cos(ang);
            double y = cy + R * std::sin(ang);
            posiciones[QString::fromStdString(sucursales[i]->getId())] = {x, y};
        }

        // Dibujar aristas
        for (const Conexion &c : red->obtenerConexiones()) {
            QPointF p1 = posiciones[QString::fromStdString(c.origenId)];
            QPointF p2 = posiciones[QString::fromStdString(c.destinoId)];

            QGraphicsLineItem *linea = scene->addLine(
                p1.x(), p1.y(), p2.x(), p2.y(),
                QPen(QColor("#90A4AE"), 2));
            linea->setZValue(0);

            // Etiqueta de peso en el punto medio
            QPointF mid = (p1 + p2) / 2.0;
            QGraphicsTextItem *etq = scene->addText(
                QString("t=%1s\nQ%2").arg(c.tiempo).arg(c.costo));
            etq->setFont(QFont("Arial", 8));
            etq->setDefaultTextColor(QColor("#455A64"));
            etq->setPos(mid.x() - 20, mid.y() - 12);
            etq->setZValue(1);
        }

        // Dibujar nodos
        const double R_nodo = 36.0;
        for (Sucursal *s : sucursales) {
            QPointF pos = posiciones[QString::fromStdString(s->getId())];
            double x = pos.x() - R_nodo;
            double y = pos.y() - R_nodo;

            QGraphicsEllipseItem *nodo = scene->addEllipse(
                x, y, R_nodo * 2, R_nodo * 2,
                QPen(QColor(AZUL_OSCURO), 2),
                QBrush(QColor(AZUL_CLARO)));
            nodo->setZValue(2);

            // ID de la sucursal
            QGraphicsTextItem *txtId = scene->addText(
                QString::fromStdString(s->getId()));
            txtId->setFont(QFont("Arial", 10, QFont::Bold));
            txtId->setDefaultTextColor(QColor(AZUL_OSCURO));
            QRectF tbr = txtId->boundingRect();
            txtId->setPos(pos.x() - tbr.width()/2, pos.y() - tbr.height()/2 - 5);
            txtId->setZValue(3);

            // Cantidad de productos
            QGraphicsTextItem *txtProd = scene->addText(
                QString("%1 prods").arg(s->contarProductos()));
            txtProd->setFont(QFont("Arial", 8));
            txtProd->setDefaultTextColor(QColor("#37474F"));
            QRectF pbr = txtProd->boundingRect();
            txtProd->setPos(pos.x() - pbr.width()/2, pos.y() + 6);
            txtProd->setZValue(3);

            // Nombre debajo del nodo
            QGraphicsTextItem *txtNom = scene->addText(
                QString::fromStdString(s->getNombre()));
            txtNom->setFont(QFont("Arial", 9));
            txtNom->setDefaultTextColor(QColor("#263238"));
            QRectF nbr = txtNom->boundingRect();
            txtNom->setPos(pos.x() - nbr.width()/2, pos.y() + R_nodo + 4);
            txtNom->setZValue(3);
        }
        view->fitInView(scene->sceneRect().adjusted(-40,-40,40,40),
                        Qt::KeepAspectRatio);
    };

    connect(btnRedibujar, &QPushButton::clicked, dibujarGrafo);

    // Agregar conexión
    connect(btnAgregarConn, &QPushButton::clicked, [=]() {
        QDialog dlg(w);
        dlg.setWindowTitle("Nueva Conexión");
        dlg.setFixedWidth(340);
        QFormLayout *fl = new QFormLayout(&dlg);
        fl->setContentsMargins(20, 20, 20, 20);

        QComboBox *cOrigen  = new QComboBox();
        QComboBox *cDestino = new QComboBox();
        for (Sucursal *s : red->obtenerSucursales()) {
            QString label = QString::fromStdString(s->getId() + " — " + s->getNombre());
            cOrigen->addItem(label, QString::fromStdString(s->getId()));
            cDestino->addItem(label, QString::fromStdString(s->getId()));
        }
        QDoubleSpinBox *dTiempo = new QDoubleSpinBox();
        dTiempo->setRange(1, 9999); dTiempo->setValue(30); dTiempo->setSuffix(" s");
        QDoubleSpinBox *dCosto  = new QDoubleSpinBox();
        dCosto->setRange(0.01, 9999); dCosto->setValue(15); dCosto->setPrefix("Q ");
        QCheckBox *chkBi = new QCheckBox("Bidireccional");
        chkBi->setChecked(true);

        fl->addRow("Origen:",        cOrigen);
        fl->addRow("Destino:",       cDestino);
        fl->addRow("Tiempo:",        dTiempo);
        fl->addRow("Costo:",         dCosto);
        fl->addRow("",               chkBi);

        QDialogButtonBox *bb = new QDialogButtonBox(
            QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
        fl->addRow(bb);
        connect(bb, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
        connect(bb, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);

        if (dlg.exec() == QDialog::Accepted) {
            QString orig = cOrigen->currentData().toString();
            QString dest = cDestino->currentData().toString();
            if (orig == dest) {
                QMessageBox::warning(w, "Error", "Origen y destino deben ser distintos.");
                return;
            }
            if (red->agregarConexion(orig.toStdString(), dest.toStdString(),
                                     dTiempo->value(), dCosto->value(),
                                     chkBi->isChecked())) {
                listaConns->addItem(
                    QString("%1 → %2  t=%3s  Q%4")
                        .arg(orig).arg(dest)
                        .arg(dTiempo->value()).arg(dCosto->value()));
                dibujarGrafo();
                emit redActualizada();
            } else {
                QMessageBox::warning(w, "Error", "No se pudo agregar la conexión.");
            }
        }
    });

    // Eliminar conexión seleccionada
    connect(btnElimConn, &QPushButton::clicked, [=]() {
        int row = listaConns->currentRow();
        if (row < 0) {
            QMessageBox::information(w, "Info", "Selecciona una conexión."); return;
        }
        QString texto = listaConns->item(row)->text();
        QString orig  = texto.split(" → ")[0].trimmed();
        QString dest  = texto.split(" → ")[1].split(" ")[0].trimmed();
        if (red->eliminarConexion(orig.toStdString(), dest.toStdString())) {
            delete listaConns->takeItem(row);
            dibujarGrafo();
            emit redActualizada();
        }
    });

    lay->addWidget(view, 1);

    // Dibujar al crear el tab
    QTimer::singleShot(100, dibujarGrafo);
    return w;
}

// ════════════════════════════════════════════════════════════
// TAB 4 — INVENTARIO
// ════════════════════════════════════════════════════════════
QWidget* MainWindow::crearTabInventario() {
    QWidget *w = new QWidget();
    QVBoxLayout *lay = new QVBoxLayout(w);
    lay->setContentsMargins(12, 12, 12, 12);
    lay->setSpacing(10);

    // Barra de búsqueda superior
    QHBoxLayout *buscLay = new QHBoxLayout();
    QComboBox *cmbSucursal = new QComboBox();
    cmbSucursal->setFixedWidth(220);
    cmbSucursal->setStyleSheet(
        "padding: 6px 8px; border: 1px solid " + GRIS_BORDE + ";"
                                                              " border-radius: 4px; font-size: 12px;");
    cmbSucursal->addItem("— Todas las sucursales —", "ALL");
    for (Sucursal *s : red->obtenerSucursales())
        cmbSucursal->addItem(
            QString::fromStdString(s->getId() + " — " + s->getNombre()),
            QString::fromStdString(s->getId()));

    QLineEdit *buscar = new QLineEdit();
    buscar->setPlaceholderText("Buscar por nombre o código de barra...");
    buscar->setStyleSheet(
        "padding: 6px 10px; border: 1px solid " + GRIS_BORDE + ";"
                                                               " border-radius: 4px; font-size: 12px;");

    QComboBox *cmbCriterio = new QComboBox();
    cmbCriterio->addItems({"Por nombre (AVL)", "Por código (Hash)",
                           "Por categoría (B+)", "Por fecha rango (B)"});
    cmbCriterio->setFixedWidth(200);
    cmbCriterio->setStyleSheet(cmbSucursal->styleSheet());

    QPushButton *btnBuscar   = new QPushButton("Buscar");
    QPushButton *btnAgregar  = new QPushButton("+ Agregar");
    QPushButton *btnEliminar = new QPushButton("✕ Eliminar");
    btnBuscar->setStyleSheet(estiloBoton(AZUL_MEDIO));
    btnAgregar->setStyleSheet(estiloBoton(VERDE));
    btnEliminar->setStyleSheet(estiloBoton(ROJO));

    buscLay->addWidget(cmbSucursal);
    buscLay->addWidget(buscar, 1);
    buscLay->addWidget(cmbCriterio);
    buscLay->addWidget(btnBuscar);
    buscLay->addWidget(btnAgregar);
    buscLay->addWidget(btnEliminar);
    lay->addLayout(buscLay);

    // Tabla de resultados
    QTableWidget *tbl = new QTableWidget();
    tbl->setColumnCount(8);
    tbl->setHorizontalHeaderLabels(
        {"Sucursal","Nombre","Código","Categoría",
         "Caducidad","Marca","Precio Q","Stock"});
    tbl->horizontalHeader()->setStretchLastSection(true);
    tbl->horizontalHeader()->setStyleSheet(estiloHeader());
    tbl->setSelectionBehavior(QAbstractItemView::SelectRows);
    tbl->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tbl->setAlternatingRowColors(true);
    tbl->verticalHeader()->setVisible(false);
    tbl->setStyleSheet("alternate-background-color: " + GRIS_CLARO + ";");
    lay->addWidget(tbl, 1);

    // Contador de resultados
    QLabel *lblConteo = new QLabel("Mostrando todos los productos");
    lblConteo->setStyleSheet("color: #555; font-size: 11px; padding: 2px 4px;");
    lay->addWidget(lblConteo);

    // ── Función para cargar todos los productos ───────────────
    auto cargarTodos = [=]() {
        tbl->setRowCount(0);
        int fila = 0;
        QString filtroSuc = cmbSucursal->currentData().toString();

        struct AVLWalker {
            static void walk(NodoAVL *n, QTableWidget *t,
                             int &r, const QString &sucId) {
                if (!n) return;
                walk(n->izquierda, t, r, sucId);
                Producto &p = n->dato;
                if (sucId == "ALL" ||
                    QString::fromStdString(p.sucursalId) == sucId) {
                    t->setRowCount(r + 1);
                    t->setItem(r,0,new QTableWidgetItem(
                                         QString::fromStdString(p.sucursalId)));
                    t->setItem(r,1,new QTableWidgetItem(
                                         QString::fromStdString(p.nombre)));
                    t->setItem(r,2,new QTableWidgetItem(
                                         QString::fromStdString(p.codigoBarra)));
                    t->setItem(r,3,new QTableWidgetItem(
                                         QString::fromStdString(p.categoria)));
                    t->setItem(r,4,new QTableWidgetItem(
                                         QString::fromStdString(p.fechaCaducidad)));
                    t->setItem(r,5,new QTableWidgetItem(
                                         QString::fromStdString(p.marca)));
                    t->setItem(r,6,new QTableWidgetItem(
                                         QString("Q%1").arg(p.precio,0,'f',2)));
                    t->setItem(r,7,new QTableWidgetItem(
                                         QString::number(p.stock)));
                    r++;
                }
                walk(n->derecha, t, r, sucId);
            }
        };

        for (Sucursal *s : red->obtenerSucursales()) {
            if (filtroSuc != "ALL" &&
                QString::fromStdString(s->getId()) != filtroSuc) continue;
            AVLWalker::walk(
                s->getCatalogo()->obtenerArbolAVL()->obtenerRaiz(),
                tbl, fila, filtroSuc);
        }
        tbl->resizeColumnsToContents();
        lblConteo->setText(QString("Mostrando %1 productos").arg(tbl->rowCount()));
    };

    cargarTodos();

    // ── Búsqueda por criterio ─────────────────────────────────
    connect(btnBuscar, &QPushButton::clicked, [=]() {
        QString texto = buscar->text().trimmed();
        if (texto.isEmpty()) { cargarTodos(); return; }

        int criterio = cmbCriterio->currentIndex();
        QString filtroSuc = cmbSucursal->currentData().toString();
        tbl->setRowCount(0);
        int fila = 0;

        for (Sucursal *s : red->obtenerSucursales()) {
            if (filtroSuc != "ALL" &&
                QString::fromStdString(s->getId()) != filtroSuc) continue;

            Catalogo *cat = s->getCatalogo();
            Producto *p = nullptr;

            if (criterio == 0) {        // AVL
                p = cat->buscarPorNombre(texto.toStdString());
            } else if (criterio == 1) { // Hash
                p = cat->buscarPorCodigo(texto.toStdString());
            }

            if (p) {
                tbl->setRowCount(fila + 1);
                tbl->setItem(fila,0,new QTableWidgetItem(QString::fromStdString(s->getId())));
                tbl->setItem(fila,1,new QTableWidgetItem(QString::fromStdString(p->nombre)));
                tbl->setItem(fila,2,new QTableWidgetItem(QString::fromStdString(p->codigoBarra)));
                tbl->setItem(fila,3,new QTableWidgetItem(QString::fromStdString(p->categoria)));
                tbl->setItem(fila,4,new QTableWidgetItem(QString::fromStdString(p->fechaCaducidad)));
                tbl->setItem(fila,5,new QTableWidgetItem(QString::fromStdString(p->marca)));
                tbl->setItem(fila,6,new QTableWidgetItem(
                                          QString("Q%1").arg(p->precio,0,'f',2)));
                tbl->setItem(fila,7,new QTableWidgetItem(QString::number(p->stock)));
                fila++;
            }
        }
        tbl->resizeColumnsToContents();
        lblConteo->setText(
            fila == 0
                ? "Sin resultados"
                : QString("%1 resultado(s) encontrado(s)").arg(fila));
    });

    connect(buscar, &QLineEdit::returnPressed, btnBuscar, &QPushButton::click);
    connect(cmbSucursal, QOverload<int>::of(&QComboBox::currentIndexChanged),
            [=](int) { cargarTodos(); });

    // ── Agregar producto ──────────────────────────────────────
    connect(btnAgregar, &QPushButton::clicked, [=]() {
        QDialog dlg(w);
        dlg.setWindowTitle("Agregar Producto");
        dlg.setFixedWidth(400);
        QFormLayout *fl = new QFormLayout(&dlg);
        fl->setContentsMargins(20, 20, 20, 20);
        fl->setSpacing(10);

        QComboBox *dSuc  = new QComboBox();
        for (Sucursal *s : red->obtenerSucursales())
            dSuc->addItem(
                QString::fromStdString(s->getId() + " — " + s->getNombre()),
                QString::fromStdString(s->getId()));

        // Preseleccionar la sucursal del filtro actual
        QString cur = cmbSucursal->currentData().toString();
        if (cur != "ALL") {
            for (int i = 0; i < dSuc->count(); i++) {
                if (dSuc->itemData(i).toString() == cur) {
                    dSuc->setCurrentIndex(i); break;
                }
            }
        }

        QLineEdit *dNom  = new QLineEdit();
        QLineEdit *dCod  = new QLineEdit(); dCod->setMaxLength(10);
        dCod->setPlaceholderText("Exactamente 10 dígitos");
        QLineEdit *dCat  = new QLineEdit();
        QLineEdit *dFec  = new QLineEdit(); dFec->setPlaceholderText("YYYY-MM-DD");
        QLineEdit *dMar  = new QLineEdit();
        QDoubleSpinBox *dPre = new QDoubleSpinBox();
        dPre->setRange(0,99999); dPre->setPrefix("Q "); dPre->setDecimals(2);
        QSpinBox *dSto = new QSpinBox(); dSto->setRange(0,99999);

        fl->addRow("Sucursal:",      dSuc);
        fl->addRow("Nombre:",        dNom);
        fl->addRow("Código (10):",   dCod);
        fl->addRow("Categoría:",     dCat);
        fl->addRow("Caducidad:",     dFec);
        fl->addRow("Marca:",         dMar);
        fl->addRow("Precio:",        dPre);
        fl->addRow("Stock:",         dSto);

        QDialogButtonBox *bb = new QDialogButtonBox(
            QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
        fl->addRow(bb);
        connect(bb, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
        connect(bb, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);

        if (dlg.exec() == QDialog::Accepted) {
            if (dCod->text().length() != 10) {
                QMessageBox::warning(w,"Error","El código debe tener exactamente 10 dígitos.");
                return;
            }
            QString sucId = dSuc->currentData().toString();
            Sucursal *s = red->buscarSucursal(sucId.toStdString());
            if (!s) return;

            Producto p(dNom->text().toStdString(),
                       dCod->text().toStdString(),
                       dCat->text().toStdString(),
                       dFec->text().toStdString(),
                       dMar->text().toStdString(),
                       dPre->value(), dSto->value(),
                       sucId.toStdString());
            if (s->agregarProducto(p)) {
                cargarTodos();
                emit redActualizada();
                QMessageBox::information(w,"OK",
                                         QString("Producto '%1' agregado.").arg(dNom->text()));
            } else {
                QMessageBox::warning(w,"Error","No se pudo agregar el producto.");
            }
        }
    });

    // ── Eliminar producto seleccionado ────────────────────────
    connect(btnEliminar, &QPushButton::clicked, [=]() {
        int row = tbl->currentRow();
        if (row < 0) {
            QMessageBox::information(w,"Info","Selecciona un producto."); return;
        }
        QString sucId  = tbl->item(row,0)->text();
        QString nombre = tbl->item(row,1)->text();
        QString codigo = tbl->item(row,2)->text();
        QString cat    = tbl->item(row,3)->text();
        QString fecha  = tbl->item(row,4)->text();

        int r = QMessageBox::question(w,"Confirmar",
                                      QString("¿Eliminar '%1' de sucursal %2?").arg(nombre).arg(sucId));
        if (r == QMessageBox::Yes) {
            Sucursal *s = red->buscarSucursal(sucId.toStdString());
            if (s && s->eliminarProducto(nombre.toStdString(),
                                         codigo.toStdString(),
                                         cat.toStdString(),
                                         fecha.toStdString())) {
                cargarTodos();
                emit redActualizada();
            } else {
                QMessageBox::warning(w,"Error","No se pudo eliminar.");
            }
        }
    });

    return w;
}

// ════════════════════════════════════════════════════════════
// TAB 5 — TRANSFERENCIA
// ════════════════════════════════════════════════════════════
QWidget* MainWindow::crearTabTransferencia() {
    QWidget *w = new QWidget();
    QHBoxLayout *lay = new QHBoxLayout(w);
    lay->setContentsMargins(12, 12, 12, 12);
    lay->setSpacing(12);

    // Panel de configuración
    QWidget *panel = new QWidget();
    panel->setFixedWidth(320);
    QVBoxLayout *pLay = new QVBoxLayout(panel);
    pLay->setSpacing(12);

    QGroupBox *gbConf = new QGroupBox("Configurar transferencia");
    gbConf->setStyleSheet(
        "QGroupBox { font-weight: bold; color: " + AZUL_OSCURO + ";"
                                                                 " border: 1px solid " + GRIS_BORDE + "; border-radius: 6px; margin-top: 8px; }"
                       "QGroupBox::title { subcontrol-origin: margin; left: 12px; padding: 0 4px; }");
    QFormLayout *fLay = new QFormLayout(gbConf);
    fLay->setSpacing(10);
    fLay->setContentsMargins(16, 16, 16, 16);

    auto comboSuc = [=]() {
        QComboBox *cb = new QComboBox();
        cb->setStyleSheet(
            "padding: 5px 8px; border: 1px solid " + GRIS_BORDE + ";"
                                                                  " border-radius: 4px; font-size: 12px;");
        for (Sucursal *s : red->obtenerSucursales())
            cb->addItem(
                QString::fromStdString(s->getId() + " — " + s->getNombre()),
                QString::fromStdString(s->getId()));
        return cb;
    };

    QComboBox *cOrigen  = comboSuc();
    QComboBox *cDestino = comboSuc();
    if (cDestino->count() > 1) cDestino->setCurrentIndex(1);

    QLineEdit *eCodigo = new QLineEdit();
    eCodigo->setPlaceholderText("Código de barra (10 dígitos)");
    eCodigo->setStyleSheet(
        "padding: 5px 8px; border: 1px solid " + GRIS_BORDE + ";"
                                                              " border-radius: 4px; font-size: 12px;");

    QRadioButton *rTiempo = new QRadioButton("Minimizar tiempo");
    QRadioButton *rCosto  = new QRadioButton("Minimizar costo");
    rTiempo->setChecked(true);
    QButtonGroup *bgCriterio = new QButtonGroup(w);
    bgCriterio->addButton(rTiempo, 0);
    bgCriterio->addButton(rCosto,  1);
    QHBoxLayout *rhLay = new QHBoxLayout();
    rhLay->addWidget(rTiempo); rhLay->addWidget(rCosto);

    fLay->addRow("Origen:",    cOrigen);
    fLay->addRow("Destino:",   cDestino);
    fLay->addRow("Código:",    eCodigo);
    fLay->addRow("Criterio:",  rhLay);

    QPushButton *btnCalcular = new QPushButton("⟶  Calcular ruta óptima");
    QPushButton *btnTransferir = new QPushButton("✓  Ejecutar transferencia");
    btnCalcular->setStyleSheet(estiloBoton(AZUL_MEDIO));
    btnTransferir->setStyleSheet(estiloBoton(VERDE));
    btnTransferir->setEnabled(false);

    fLay->addRow("", btnCalcular);
    fLay->addRow("", btnTransferir);
    pLay->addWidget(gbConf);

    // Estado de colas
    QGroupBox *gbColas = new QGroupBox("Estado de colas");
    gbColas->setStyleSheet(gbConf->styleSheet());
    QVBoxLayout *colasLay = new QVBoxLayout(gbColas);
    QTextEdit *txtColas = new QTextEdit();
    txtColas->setReadOnly(true);
    txtColas->setStyleSheet(
        "font-family: Courier New; font-size: 11px;"
        " border: 1px solid " + GRIS_BORDE + ";");
    txtColas->setFixedHeight(160);

    // Mostrar estado de colas de todas las sucursales
    QString infoColasTxt;
    for (Sucursal *s : red->obtenerSucursales()) {
        infoColasTxt += QString("[%1]\n  Ingreso:  %2\n  Traspaso: %3\n  Salida:   %4\n\n")
        .arg(QString::fromStdString(s->getId()))
            .arg(s->getColaIngreso().obtenerTamano())
            .arg(s->getColaTraspaso().obtenerTamano())
            .arg(s->getColaSalida().obtenerTamano());
    }
    txtColas->setText(infoColasTxt);
    colasLay->addWidget(txtColas);
    pLay->addWidget(gbColas);
    pLay->addStretch();
    lay->addWidget(panel);

    // Panel de resultados
    QWidget *panelRes = new QWidget();
    QVBoxLayout *rLay = new QVBoxLayout(panelRes);
    rLay->setSpacing(12);

    // Grafo de la ruta
    QGroupBox *gbRuta = new QGroupBox("Ruta óptima");
    gbRuta->setStyleSheet(gbConf->styleSheet());
    QVBoxLayout *rutaLay = new QVBoxLayout(gbRuta);

    QGraphicsScene *sceneRuta = new QGraphicsScene();
    sceneRuta->setBackgroundBrush(Qt::white);
    QGraphicsView *viewRuta = new QGraphicsView(sceneRuta);
    viewRuta->setRenderHint(QPainter::Antialiasing);
    viewRuta->setFixedHeight(220);
    viewRuta->setStyleSheet("border: 1px solid " + GRIS_BORDE + ";");
    rutaLay->addWidget(viewRuta);
    rLay->addWidget(gbRuta);

    // Detalles de la ruta
    QGroupBox *gbDetalle = new QGroupBox("Detalles");
    gbDetalle->setStyleSheet(gbConf->styleSheet());
    QVBoxLayout *detLay = new QVBoxLayout(gbDetalle);
    QTextEdit *txtDetalle = new QTextEdit();
    txtDetalle->setReadOnly(true);
    txtDetalle->setStyleSheet(
        "font-family: Courier New; font-size: 12px;"
        " border: 1px solid " + GRIS_BORDE + ";");
    detLay->addWidget(txtDetalle);
    rLay->addWidget(gbDetalle, 1);
    lay->addWidget(panelRes, 1);

    // Almacenar la última ruta calculada
    struct EstadoRuta {
        ResultadoRuta ruta;
        QString       codigoBarra;
        QString       origenId;
        QString       destinoId;
    };
    EstadoRuta *estado = new EstadoRuta();

    // ── Calcular ruta ─────────────────────────────────────────
    connect(btnCalcular, &QPushButton::clicked, [=]() {
        QString orig  = cOrigen->currentData().toString();
        QString dest  = cDestino->currentData().toString();
        QString codigo = eCodigo->text().trimmed();

        if (orig == dest) {
            QMessageBox::warning(w,"Error","Origen y destino deben ser distintos."); return;
        }

        RedSucursales::Criterio crit = rTiempo->isChecked()
                                           ? RedSucursales::TIEMPO : RedSucursales::COSTO;

        ResultadoRuta ruta = red->rutaOptima(orig.toStdString(),
                                             dest.toStdString(), crit);
        estado->ruta       = ruta;
        estado->codigoBarra = codigo;
        estado->origenId   = orig;
        estado->destinoId  = dest;

        // Dibujar la ruta en el grafo
        sceneRuta->clear();
        if (!ruta.encontrada) {
            QGraphicsTextItem *t = sceneRuta->addText("No existe ruta entre las sucursales.");
            t->setDefaultTextColor(Qt::red);
            txtDetalle->setText("Sin ruta encontrada.");
            btnTransferir->setEnabled(false);
            return;
        }

        // Nodos de la ruta en línea horizontal
        int n = (int)ruta.nodos.size();
        const double espacio = 140.0;
        const double cy = 80.0;
        double xInicio = 60.0;

        for (int i = 0; i < n; i++) {
            double x = xInicio + i * espacio;
            Sucursal *s = red->buscarSucursal(ruta.nodos[i]);
            QColor color = (i == 0 || i == n-1) ? QColor("#FFD600") : QColor(AZUL_CLARO);

            sceneRuta->addEllipse(x - 30, cy - 30, 60, 60,
                                  QPen(QColor(AZUL_OSCURO), 2), QBrush(color));

            QGraphicsTextItem *tid = sceneRuta->addText(
                QString::fromStdString(ruta.nodos[i]));
            tid->setFont(QFont("Arial", 9, QFont::Bold));
            tid->setDefaultTextColor(QColor(AZUL_OSCURO));
            tid->setPos(x - tid->boundingRect().width()/2, cy - 10);

            if (s) {
                QGraphicsTextItem *tnom = sceneRuta->addText(
                    QString::fromStdString(s->getNombre()));
                tnom->setFont(QFont("Arial", 8));
                tnom->setPos(x - 50, cy + 36);
            }

            if (i < n - 1) {
                // Flecha entre nodos
                sceneRuta->addLine(x + 30, cy, xInicio + (i+1)*espacio - 30, cy,
                                   QPen(QColor("#E53935"), 2.5));
                // Peso
                auto conns = red->obtenerConexionesDe(ruta.nodos[i]);
                for (const Conexion &c : conns) {
                    if (c.destinoId == ruta.nodos[i+1]) {
                        QGraphicsTextItem *tp = sceneRuta->addText(
                            QString("t=%1s\nQ%2").arg(c.tiempo).arg(c.costo));
                        tp->setFont(QFont("Arial", 8));
                        tp->setDefaultTextColor(QColor("#37474F"));
                        tp->setPos(x + 32, cy - 24);
                        break;
                    }
                }
            }
        }
        viewRuta->fitInView(sceneRuta->sceneRect().adjusted(-20,-20,20,20),
                            Qt::KeepAspectRatio);

        // Detalle textual
        double eta = red->calcularETA(ruta, crit);
        QString detTxt;
        detTxt += QString("=== Ruta óptima (%1) ===\n")
                      .arg(crit == RedSucursales::TIEMPO ? "mínimo tiempo" : "mínimo costo");
        detTxt += "Camino: ";
        for (const auto &nid : ruta.nodos)
            detTxt += QString::fromStdString(nid) + " → ";
        detTxt.chop(3);
        detTxt += "\n\n";
        detTxt += QString("Peso total  : %1 %2\n")
                      .arg(ruta.pesoTotal)
                      .arg(crit == RedSucursales::TIEMPO ? "segundos" : "Q");
        detTxt += QString("ETA total   : %1 segundos\n").arg(eta);
        detTxt += QString("Nodos en ruta: %1\n\n").arg(n);
        detTxt += "Detalle por tramo:\n";

        for (int i = 0; i < n - 1; i++) {
            auto conns = red->obtenerConexionesDe(ruta.nodos[i]);
            for (const Conexion &c : conns) {
                if (c.destinoId == ruta.nodos[i+1]) {
                    detTxt += QString("  %1 → %2 : t=%3s  Q%4\n")
                                  .arg(QString::fromStdString(c.origenId))
                                  .arg(QString::fromStdString(c.destinoId))
                                  .arg(c.tiempo).arg(c.costo);
                    break;
                }
            }
        }

        if (!codigo.isEmpty()) {
            Sucursal *sOrig = red->buscarSucursal(orig.toStdString());
            Producto *p = sOrig ? sOrig->buscarPorCodigo(codigo.toStdString()) : nullptr;
            if (p) {
                detTxt += QString("\nProducto: %1\n  Código: %2\n  Estado: %3\n")
                              .arg(QString::fromStdString(p->nombre))
                              .arg(codigo)
                              .arg(QString::fromStdString(p->estado));
                btnTransferir->setEnabled(true);
            } else {
                detTxt += "\n[!] Producto no encontrado en el origen.\n";
                btnTransferir->setEnabled(false);
            }
        } else {
            detTxt += "\n[!] Ingresa un código para habilitar la transferencia.\n";
            btnTransferir->setEnabled(false);
        }
        txtDetalle->setText(detTxt);
    });

    // ── Ejecutar transferencia ────────────────────────────────
    connect(btnTransferir, &QPushButton::clicked, [=]() {
        if (!estado->ruta.encontrada) return;

        RedSucursales::Criterio crit = rTiempo->isChecked()
                                           ? RedSucursales::TIEMPO : RedSucursales::COSTO;

        bool ok = red->transferirProducto(
            estado->codigoBarra.toStdString(),
            estado->origenId.toStdString(),
            estado->destinoId.toStdString(),
            crit);

        if (ok) {
            QMessageBox::information(w, "Transferencia exitosa",
                                     QString("Producto transferido de %1 a %2.")
                                         .arg(estado->origenId).arg(estado->destinoId));
            btnTransferir->setEnabled(false);
            emit redActualizada();
        } else {
            QMessageBox::warning(w, "Error", "No se pudo completar la transferencia.");
        }
    });

    return w;
}

// ════════════════════════════════════════════════════════════
// TAB 6 — RENDIMIENTO
// ════════════════════════════════════════════════════════════
QWidget* MainWindow::crearTabRendimiento() {
    QWidget *w = new QWidget();
    QVBoxLayout *lay = new QVBoxLayout(w);
    lay->setContentsMargins(24, 20, 24, 20);
    lay->setSpacing(16);

    QLabel *titulo = new QLabel("Medición de Rendimiento — Búsqueda por Nombre");
    titulo->setStyleSheet(
        "color: " + AZUL_OSCURO + "; font-size: 16px; font-weight: bold;"
                                  " border-bottom: 2px solid " + AZUL_MEDIO + "; padding-bottom: 8px;");
    lay->addWidget(titulo);

    // Configuración
    QHBoxLayout *confLay = new QHBoxLayout();
    QLabel *lblSuc = new QLabel("Sucursal:");
    QComboBox *cmbSuc = new QComboBox();
    cmbSuc->setStyleSheet(
        "padding: 6px; border: 1px solid " + GRIS_BORDE + ";"
                                                          " border-radius: 4px; font-size: 12px;");
    for (Sucursal *s : red->obtenerSucursales())
        cmbSuc->addItem(
            QString::fromStdString(s->getId() + " — " + s->getNombre()),
            QString::fromStdString(s->getId()));

    QPushButton *btnEjecutar = new QPushButton("▶  Ejecutar benchmark");
    btnEjecutar->setStyleSheet(estiloBoton(AZUL_MEDIO));
    btnEjecutar->setFixedWidth(200);

    confLay->addWidget(lblSuc);
    confLay->addWidget(cmbSuc, 1);
    confLay->addWidget(btnEjecutar);
    lay->addLayout(confLay);

    // Tabla de resultados
    QTableWidget *tbl = new QTableWidget(4, 5);
    tbl->setHorizontalHeaderLabels(
        {"Caso de prueba","Lista Simple (µs)",
         "Lista Ordenada (µs)","Árbol AVL (µs)","Hash (µs)"});
    tbl->verticalHeader()->setVisible(false);
    tbl->horizontalHeader()->setStretchLastSection(true);
    tbl->horizontalHeader()->setStyleSheet(estiloHeader());
    tbl->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tbl->setAlternatingRowColors(true);
    tbl->setStyleSheet("alternate-background-color: " + GRIS_CLARO + ";");

    QStringList casos = {
        "Búsqueda exitosa aleatoria",
        "Búsqueda fallida",
        "Extremos (primero/último)",
        "Peor caso (último A-Z)"
    };
    for (int i = 0; i < 4; i++) {
        tbl->setItem(i, 0, new QTableWidgetItem(casos[i]));
        for (int j = 1; j < 5; j++)
            tbl->setItem(i, j, new QTableWidgetItem("—"));
    }
    tbl->resizeColumnsToContents();
    lay->addWidget(tbl);

    // Análisis
    QGroupBox *gbAnalisis = new QGroupBox("Análisis de factores de aceleración");
    gbAnalisis->setStyleSheet(
        "QGroupBox { font-weight: bold; color: " + AZUL_OSCURO + ";"
                                                                 " border: 1px solid " + GRIS_BORDE + "; border-radius: 6px; margin-top: 8px; }"
                       "QGroupBox::title { subcontrol-origin: margin; left: 12px; padding: 0 4px; }");
    QVBoxLayout *aLay = new QVBoxLayout(gbAnalisis);
    QTextEdit *txtAnalisis = new QTextEdit();
    txtAnalisis->setReadOnly(true);
    txtAnalisis->setStyleSheet(
        "font-family: Courier New; font-size: 12px;"
        " border: 1px solid " + GRIS_BORDE + ";");
    txtAnalisis->setFixedHeight(140);
    aLay->addWidget(txtAnalisis);
    lay->addWidget(gbAnalisis);
    lay->addStretch();

    // ── Ejecutar benchmark ────────────────────────────────────
    connect(btnEjecutar, &QPushButton::clicked, [=]() {
        QString sucId = cmbSuc->currentData().toString();
        Sucursal *s = red->buscarSucursal(sucId.toStdString());
        if (!s || s->contarProductos() < 20) {
            QMessageBox::warning(w,"Error","Se necesitan al menos 20 productos."); return;
        }

        // Recopilar nombres del AVL
        std::vector<std::string> nombres;
        struct Walker {
            static void walk(NodoAVL *n, std::vector<std::string> &v) {
                if (!n || v.size() >= 20) return;
                walk(n->izquierda, v);
                if (v.size() < 20) v.push_back(n->dato.nombre);
                walk(n->derecha, v);
            }
        };
        Walker::walk(s->getCatalogo()->obtenerArbolAVL()->obtenerRaiz(), nombres);

        if (nombres.size() < 20) {
            QMessageBox::warning(w,"Error","No hay suficientes productos."); return;
        }

        const int N = 20, M = 5;
        using Clock = std::chrono::high_resolution_clock;

        auto medirLS = [&](const std::vector<std::string> &ns) {
            double total = 0;
            for (int rep = 0; rep < M; rep++) {
                auto t0 = Clock::now();
                for (int i = 0; i < N; i++)
                    s->getCatalogo()->obtenerListaSimple()->buscarPorNombre(ns[i % ns.size()]);
                total += std::chrono::duration<double,std::micro>(Clock::now()-t0).count();
            }
            return total / M / N;
        };
        auto medirLO = [&](const std::vector<std::string> &ns) {
            double total = 0;
            for (int rep = 0; rep < M; rep++) {
                auto t0 = Clock::now();
                for (int i = 0; i < N; i++)
                    s->getCatalogo()->obtenerListaOrdenada()->buscarPorNombre(ns[i % ns.size()]);
                total += std::chrono::duration<double,std::micro>(Clock::now()-t0).count();
            }
            return total / M / N;
        };
        auto medirAVL = [&](const std::vector<std::string> &ns) {
            double total = 0;
            for (int rep = 0; rep < M; rep++) {
                auto t0 = Clock::now();
                for (int i = 0; i < N; i++)
                    s->getCatalogo()->obtenerArbolAVL()->buscar(ns[i % ns.size()]);
                total += std::chrono::duration<double,std::micro>(Clock::now()-t0).count();
            }
            return total / M / N;
        };
        auto medirHash = [&](const std::vector<std::string> & /*ns*/) {
            // Hash busca por código, así que tomamos el código del primer producto
            std::vector<std::string> codigos;
            struct CWalk {
                static void walk(NodoAVL *n, std::vector<std::string> &v) {
                    if (!n || v.size() >= 20) return;
                    walk(n->izquierda, v);
                    if (v.size() < 20) v.push_back(n->dato.codigoBarra);
                    walk(n->derecha, v);
                }
            };
            CWalk::walk(s->getCatalogo()->obtenerArbolAVL()->obtenerRaiz(), codigos);
            double total = 0;
            for (int rep = 0; rep < M; rep++) {
                auto t0 = Clock::now();
                for (int i = 0; i < N; i++)
                    s->getCatalogo()->obtenerTablaHash()->buscar(codigos[i % codigos.size()]);
                total += std::chrono::duration<double,std::micro>(Clock::now()-t0).count();
            }
            return total / M / N;
        };

        std::vector<std::string> fallidas = {
            "Zzzzz No Existe", "XXXXX Fantasma",
            "Producto Inexistente", "AAA Falso", "ZZZ Nada"
        };
        std::vector<std::string> extremos = {nombres.front(), nombres.back()};
        std::vector<std::string> peor     = {nombres.back()};

        struct { double ls, lo, avl, hash; } res[4];
        res[0] = {medirLS(nombres),   medirLO(nombres),   medirAVL(nombres),  medirHash(nombres)};
        res[1] = {medirLS(fallidas),  medirLO(fallidas),  medirAVL(fallidas), medirHash(fallidas)};
        res[2] = {medirLS(extremos),  medirLO(extremos),  medirAVL(extremos), medirHash(extremos)};
        res[3] = {medirLS(peor),      medirLO(peor),      medirAVL(peor),     medirHash(peor)};

        for (int i = 0; i < 4; i++) {
            tbl->item(i,1)->setText(QString::number(res[i].ls,  'f', 3));
            tbl->item(i,2)->setText(QString::number(res[i].lo,  'f', 3));
            tbl->item(i,3)->setText(QString::number(res[i].avl, 'f', 3));
            tbl->item(i,4)->setText(QString::number(res[i].hash,'f', 3));
        }
        tbl->resizeColumnsToContents();

        // Análisis
        double avl0  = res[0].avl  > 0 ? res[0].avl  : 0.001;
        double hash0 = res[0].hash > 0 ? res[0].hash : 0.001;
        QString txt;
        txt += QString("=== Factores de aceleración (búsqueda exitosa) ===\n");
        txt += QString("Lista Simple  vs AVL  : %.1fx más lenta\n").arg(res[0].ls  / avl0);
        txt += QString("Lista Ordenada vs AVL : %.1fx más lenta\n").arg(res[0].lo  / avl0);
        txt += QString("Lista Simple  vs Hash : %.1fx más lenta\n").arg(res[0].ls  / hash0);
        txt += QString("AVL vs Hash           : %.1fx (AVL %s)\n")
                   .arg(avl0 / hash0)
                   .arg(avl0 > hash0 ? "más lento" : "más rápido");
        txt += QString("\nn=%1 → log₂(n)≈%.1f niveles en AVL | Hash O(1) amortizado\n")
                   .arg(s->contarProductos())
                   .arg(std::log2(s->contarProductos()));
        txtAnalisis->setText(txt);
    });

    return w;
}

// ════════════════════════════════════════════════════════════
// TAB 7 — ESTRUCTURAS (Visualización)
// ════════════════════════════════════════════════════════════
QWidget* MainWindow::crearTabEstructuras() {
    QWidget *w = new QWidget();
    QVBoxLayout *lay = new QVBoxLayout(w);
    lay->setContentsMargins(12, 12, 12, 12);
    lay->setSpacing(10);

    QHBoxLayout *ctrlLay = new QHBoxLayout();
    QComboBox *cmbSuc = new QComboBox();
    cmbSuc->setStyleSheet(
        "padding: 6px; border: 1px solid " + GRIS_BORDE + ";"
                                                          " border-radius: 4px; font-size: 12px;");
    for (Sucursal *s : red->obtenerSucursales())
        cmbSuc->addItem(
            QString::fromStdString(s->getId() + " — " + s->getNombre()),
            QString::fromStdString(s->getId()));

    QComboBox *cmbArbol = new QComboBox();
    cmbArbol->addItems({"Árbol AVL", "Árbol B", "Árbol B+", "Grafo de red"});
    cmbArbol->setStyleSheet(cmbSuc->styleSheet());
    cmbArbol->setFixedWidth(160);

    QPushButton *btnGenerar = new QPushButton("⟳  Generar .dot");
    QPushButton *btnConvertir = new QPushButton("🖼  Convertir a PNG");
    btnGenerar->setStyleSheet(estiloBoton(AZUL_MEDIO));
    btnConvertir->setStyleSheet(estiloBoton(NARANJA));

    ctrlLay->addWidget(new QLabel("Sucursal:"));
    ctrlLay->addWidget(cmbSuc, 1);
    ctrlLay->addWidget(new QLabel("Árbol:"));
    ctrlLay->addWidget(cmbArbol);
    ctrlLay->addWidget(btnGenerar);
    ctrlLay->addWidget(btnConvertir);
    lay->addLayout(ctrlLay);

    QLabel *lblImg = new QLabel("Genera el archivo .dot y conviértelo a PNG para visualizar.");
    lblImg->setAlignment(Qt::AlignCenter);
    lblImg->setStyleSheet(
        "color: #888; font-size: 13px; background: " + GRIS_CLARO + ";"
                                                                    " border: 1px solid " + GRIS_BORDE + "; border-radius: 4px; padding: 20px;");
    lblImg->setWordWrap(true);
    lay->addWidget(lblImg, 1);

    QLabel *lblRuta = new QLabel();
    lblRuta->setStyleSheet("color: #555; font-size: 11px; padding: 2px;");
    lay->addWidget(lblRuta);

    // ── Generar .dot ──────────────────────────────────────────
    connect(btnGenerar, &QPushButton::clicked, [=]() {
        QString sucId  = cmbSuc->currentData().toString();
        int     arbol  = cmbArbol->currentIndex();
        Sucursal *s    = red->buscarSucursal(sucId.toStdString());

        QString rutaDot;
        if (arbol == 3) { // Grafo de red
            rutaDot = "output/grafo_red.dot";
            red->generarDot(rutaDot.toStdString());
        } else if (s) {
            VisualizadorDot viz("output");
            Catalogo *cat = s->getCatalogo();
            if (arbol == 0) {
                rutaDot = "output/avl.dot";
                viz.generarAVL(cat->obtenerArbolAVL(), 30);
            } else if (arbol == 1) {
                rutaDot = "output/arbolB.dot";
                viz.generarArbolB(cat->obtenerArbolB(), 20);
            } else {
                rutaDot = "output/arbolBP.dot";
                viz.generarArbolBPlus(cat->obtenerArbolBPlus(), 20);
            }
        }
        lblRuta->setText("Archivo generado: " + rutaDot);
        lblImg->setText(
            QString("Archivo .dot generado en:\n%1\n\n"
                    "Para convertir a PNG, presiona 'Convertir a PNG'\n"
                    "(requiere Graphviz instalado en el sistema).")
                .arg(rutaDot));
    });

    // ── Convertir a PNG con Graphviz ──────────────────────────
    connect(btnConvertir, &QPushButton::clicked, [=]() {
        QString rutaDot = lblRuta->text().replace("Archivo generado: ", "");
        if (rutaDot.isEmpty()) {
            QMessageBox::information(w, "Info", "Primero genera el archivo .dot."); return;
        }
        QString rutaPng = rutaDot;
        rutaPng.replace(".dot", ".png");

        QProcess proc;
        proc.start("dot", {"-Tpng", rutaDot, "-o", rutaPng});
        proc.waitForFinished(8000);

        if (proc.exitCode() == 0) {
            QPixmap px(rutaPng);
            if (!px.isNull()) {
                lblImg->setPixmap(px.scaled(lblImg->size(),
                                            Qt::KeepAspectRatio, Qt::SmoothTransformation));
                lblImg->setText("");
            } else {
                lblImg->setText("PNG generado en: " + rutaPng);
            }
        } else {
            QMessageBox::warning(w, "Error",
                                 "No se pudo convertir. ¿Está Graphviz instalado?\n"
                                 "Instalar desde: https://graphviz.org/download/\n\n"
                                 "También puedes abrir el .dot manualmente en:\n" + rutaDot);
        }
    });

    return w;
}
