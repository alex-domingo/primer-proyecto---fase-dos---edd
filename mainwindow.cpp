#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "VisualizadorDot.h"
#include "SimuladorTransferencia.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QFormLayout>
#include <QSplitter>
#include <QScrollArea>
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
#include <QDialog>
#include <QDialogButtonBox>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsEllipseItem>
#include <QGraphicsLineItem>
#include <QGraphicsTextItem>
#include <QFileDialog>
#include <QPixmap>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QRadioButton>
#include <QButtonGroup>
#include <QCheckBox>
#include <QTimer>
#include <QProcess>
#include <QProgressBar>
#include <QDateTime>
#include <QString>
#include <QFont>
#include <QColor>
#include <chrono>
#include <cmath>

// ── Paleta y estilos del sistema ──────────────────────────────
static const QString AZUL_OSCURO = "#1F3864";
static const QString AZUL_MEDIO  = "#2E75B6";
static const QString AZUL_CLARO  = "#BDD7EE";
static const QString VERDE       = "#1E6B2E";
static const QString VERDE_CLARO = "#E8F5E9";
static const QString ROJO        = "#8B0000";
static const QString GRIS_CLARO  = "#F5F5F5";
static const QString GRIS_BORDE  = "#DDDDDD";
static const QString AMARILLO    = "#FFF9C4";
static const QString NARANJA     = "#E65100";
static const QString TEXTO_NEGRO = "#1A1A1A"; // texto principal

QString MainWindow::estiloBoton(const QString &color) {
    return QString(
               "QPushButton {"
               "  background-color: %1; color: white; border: none;"
               "  border-radius: 4px; padding: 7px 16px; font-size: 12px; font-weight: bold;"
               "}"
               "QPushButton:hover { background-color: %1; border: 1px solid white; }"
               "QPushButton:pressed { padding: 8px 15px 6px 17px; }"
               "QPushButton:disabled { background-color: #B0B0B0; color: #E0E0E0; }"
               ).arg(color);
}

QString MainWindow::estiloHeader() {
    return QString(
               "QHeaderView::section {"
               "  background-color: %1; color: white;"
               "  padding: 7px 10px; font-weight: bold; font-size: 12px;"
               "  border: none; border-right: 1px solid white;"
               "}"
               ).arg(AZUL_OSCURO);
}

QString MainWindow::estiloTabla() {
    // Estilo completo de QTableWidget — texto negro garantizado
    return QString(
               "QTableWidget {"
               "  background-color: white; color: %1;"
               "  alternate-background-color: %2;"
               "  gridline-color: %3; font-size: 12px;"
               "  selection-background-color: %4; selection-color: white;"
               "}"
               "QTableWidget::item { padding: 4px 6px; color: %1; }"
               "QTableWidget::item:selected { color: white; }"
               ).arg(TEXTO_NEGRO).arg(GRIS_CLARO).arg(GRIS_BORDE).arg(AZUL_MEDIO);
}

QString MainWindow::estiloCampo() {
    return QString(
               "QLineEdit, QSpinBox, QDoubleSpinBox, QComboBox {"
               "  background: white; color: %1;"
               "  border: 1px solid %2; border-radius: 4px;"
               "  padding: 5px 8px; font-size: 12px;"
               "}"
               "QLineEdit:focus, QSpinBox:focus, QDoubleSpinBox:focus, QComboBox:focus {"
               "  border: 1px solid %3;"
               "}"
               "QLineEdit:read-only { background: %4; }"
               ).arg(TEXTO_NEGRO).arg(GRIS_BORDE).arg(AZUL_MEDIO).arg(GRIS_CLARO);
}

static QString estiloGroupBox() {
    return QString(
               "QGroupBox {"
               "  font-weight: bold; font-size: 13px; color: %1;"
               "  background: white;"
               "  border: 1px solid %2; border-radius: 6px;"
               "  margin-top: 10px; padding-top: 8px;"
               "}"
               "QGroupBox::title {"
               "  subcontrol-origin: margin; left: 12px; padding: 0 6px;"
               "  background: white; color: %1;"
               "}"
               ).arg(AZUL_OSCURO).arg(GRIS_BORDE);
}

static QString estiloLista() {
    return QString(
               "QListWidget {"
               "  background: white; color: %1;"
               "  border: 1px solid %2; font-size: 12px;"
               "}"
               "QListWidget::item { padding: 8px; border-bottom: 1px solid %3; color: %1; }"
               "QListWidget::item:selected { background: %4; color: white; }"
               ).arg(TEXTO_NEGRO).arg(GRIS_BORDE).arg(GRIS_CLARO).arg(AZUL_MEDIO);
}

static QString estiloTextEdit() {
    return QString(
               "QTextEdit {"
               "  background: white; color: %1;"
               "  border: 1px solid %2;"
               "  font-family: 'Courier New'; font-size: 12px;"
               "}"
               ).arg(TEXTO_NEGRO).arg(GRIS_BORDE);
}

// ── Constructor ───────────────────────────────────────────────

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , red(new RedSucursales())
{
    ui->setupUi(this);
    setWindowTitle("Gestión de Catálogo — Red de Sucursales");
    resize(1280, 820);
    setMinimumSize(1100, 700);

    // Forzamos paleta clara para evitar que el tema oscuro de Windows
    // pinte texto blanco sobre fondo blanco
    QPalette pal;
    pal.setColor(QPalette::Window,        Qt::white);
    pal.setColor(QPalette::WindowText,    QColor(TEXTO_NEGRO));
    pal.setColor(QPalette::Base,          Qt::white);
    pal.setColor(QPalette::AlternateBase, QColor(GRIS_CLARO));
    pal.setColor(QPalette::Text,          QColor(TEXTO_NEGRO));
    pal.setColor(QPalette::ButtonText,    QColor(TEXTO_NEGRO));
    pal.setColor(QPalette::ToolTipBase,   Qt::white);
    pal.setColor(QPalette::ToolTipText,   QColor(TEXTO_NEGRO));
    setPalette(pal);

    // El sistema arranca VACÍO — el usuario carga los CSV manualmente
    construirUI();
}

MainWindow::~MainWindow() {
    delete red;
    delete ui;
}

// ── Construcción de la UI principal ──────────────────────────

void MainWindow::construirUI() {
    QWidget *central = new QWidget(this);
    central->setStyleSheet("background: white;");
    setCentralWidget(central);
    QVBoxLayout *mainLayout = new QVBoxLayout(central);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // Barra de título
    QFrame *header = new QFrame();
    header->setFixedHeight(52);
    header->setStyleSheet(QString(
                              "background-color: %1; border-bottom: 3px solid %2;")
                              .arg(AZUL_OSCURO).arg(AZUL_MEDIO));
    QHBoxLayout *hl = new QHBoxLayout(header);
    hl->setContentsMargins(16, 0, 16, 0);

    QLabel *titulo = new QLabel("  Gestión de Catálogo — Red de Sucursales");
    titulo->setStyleSheet("color: white; font-size: 16px; font-weight: bold;");
    hl->addWidget(titulo);
    hl->addStretch();

    lblHeaderStats = new QLabel();
    lblHeaderStats->setStyleSheet("color: #AAD4FF; font-size: 12px;");
    hl->addWidget(lblHeaderStats);
    mainLayout->addWidget(header);

    // Tabs
    tabs = new QTabWidget();
    tabs->setStyleSheet(QString(
                            "QTabWidget::pane { border: 1px solid %1; background: white; }"
                            "QTabBar::tab {"
                            "  min-width: 120px; padding: 9px 16px;"
                            "  font-size: 12px; font-weight: bold;"
                            "  color: #444; background: #EEEEEE;"
                            "}"
                            "QTabBar::tab:selected { background: %2; color: white; }"
                            "QTabBar::tab:hover:!selected { background: #DDDDDD; }"
                            ).arg(GRIS_BORDE).arg(AZUL_MEDIO));

    tabs->addTab(crearTabSistema(),       "  Sistema  ");
    tabs->addTab(crearTabSucursales(),    "  Sucursales  ");
    tabs->addTab(crearTabRed(),           "  Red  ");
    tabs->addTab(crearTabInventario(),    "  Inventario  ");
    tabs->addTab(crearTabTransferencia(), "  Transferencia  ");
    tabs->addTab(crearTabRendimiento(),   "  Rendimiento  ");
    tabs->addTab(crearTabEstructuras(),   "  Visualización  ");

    mainLayout->addWidget(tabs);

    // Barra de estado
    lblEstado = new QLabel();
    statusBar()->addWidget(lblEstado);
    statusBar()->setStyleSheet(QString(
                                   "QStatusBar { background: %1; color: #AAD4FF; font-size: 11px; }"
                                   "QStatusBar QLabel { color: #AAD4FF; }"
                                   ).arg(AZUL_OSCURO));

    connect(this, &MainWindow::redActualizada,
            this, &MainWindow::onRedActualizada);

    actualizarBarraEstado();
}

void MainWindow::actualizarBarraEstado() {
    int total = 0;
    for (auto s : red->obtenerSucursales()) total += s->contarProductos();

    QString msg;
    if (red->contarSucursales() == 0) {
        msg = "  Sistema iniciado — red vacía. Usa los botones de carga en el Tab Sistema.";
    } else {
        msg = QString("  %1 sucursales | %2 conexiones | %3 productos | 6 estructuras por sucursal  ")
        .arg(red->contarSucursales())
            .arg(red->contarConexiones())
            .arg(total);
    }
    lblEstado->setText(msg);
    lblHeaderStats->setText(
        QString("  %1 sucursales | %2 conexiones | %3 productos  ")
            .arg(red->contarSucursales())
            .arg(red->contarConexiones())
            .arg(total));
}

void MainWindow::onRedActualizada() {
    actualizarBarraEstado();
}

// ════════════════════════════════════════════════════════════
// TAB 1 — SISTEMA (con carga manual de CSV)
// ════════════════════════════════════════════════════════════
QWidget* MainWindow::crearTabSistema() {
    QWidget *w = new QWidget();
    w->setStyleSheet("background: white;");
    QVBoxLayout *lay = new QVBoxLayout(w);
    lay->setContentsMargins(20, 20, 20, 20);
    lay->setSpacing(14);

    QLabel *titulo = new QLabel("Estado del Sistema");
    titulo->setStyleSheet(QString(
                              "color: %1; font-size: 18px; font-weight: bold;"
                              " border-bottom: 2px solid %2; padding-bottom: 8px;"
                              " background: white;"
                              ).arg(AZUL_OSCURO).arg(AZUL_MEDIO));
    lay->addWidget(titulo);

    // ── Tarjetas de estadísticas ──────────────────────────────
    QHBoxLayout *cards = new QHBoxLayout();
    cards->setSpacing(12);

    auto crearTarjeta = [](const QString &color, const QString &lblText) {
        QFrame *f = new QFrame();
        f->setStyleSheet(QString(
                             "QFrame { background: %1; border-radius: 8px;"
                             " border: 1px solid %2; }"
                             ).arg(color).arg(GRIS_BORDE));
        f->setFixedHeight(90);
        QVBoxLayout *fl = new QVBoxLayout(f);
        fl->setAlignment(Qt::AlignCenter);
        QLabel *lv = new QLabel("0");
        lv->setAlignment(Qt::AlignCenter);
        lv->setStyleSheet(QString(
                              "color: %1; font-size: 28px; font-weight: bold; background: transparent;"
                              ).arg(AZUL_OSCURO));
        QLabel *le = new QLabel(lblText);
        le->setAlignment(Qt::AlignCenter);
        le->setStyleSheet("color: #555; font-size: 11px; background: transparent;");
        fl->addWidget(lv); fl->addWidget(le);
        f->setProperty("valLabel", QVariant::fromValue((void*)lv));
        return f;
    };

    QFrame *tSuc  = crearTarjeta(AZUL_CLARO, "Sucursales");
    QFrame *tCon  = crearTarjeta(AZUL_CLARO, "Conexiones");
    QFrame *tProd = crearTarjeta(VERDE_CLARO, "Productos totales");
    QFrame *tEst  = crearTarjeta(AMARILLO, "Estructuras por sucursal");
    cards->addWidget(tSuc); cards->addWidget(tCon);
    cards->addWidget(tProd); cards->addWidget(tEst);
    lay->addLayout(cards);

    auto actualizarTarjetas = [=]() {
        int total = 0;
        for (auto s : red->obtenerSucursales()) total += s->contarProductos();
        ((QLabel*)tSuc->property("valLabel").value<void*>())->setText(
            QString::number(red->contarSucursales()));
        ((QLabel*)tCon->property("valLabel").value<void*>())->setText(
            QString::number(red->contarConexiones()));
        ((QLabel*)tProd->property("valLabel").value<void*>())->setText(
            QString::number(total));
        ((QLabel*)tEst->property("valLabel").value<void*>())->setText("6");
    };
    actualizarTarjetas();

    // ── Carga de CSV ──────────────────────────────────────────
    QGroupBox *gbCarga = new QGroupBox("Carga de archivos CSV");
    gbCarga->setStyleSheet(estiloGroupBox());
    QVBoxLayout *cLay = new QVBoxLayout(gbCarga);
    cLay->setContentsMargins(16, 16, 16, 16);
    cLay->setSpacing(8);

    QLabel *info = new QLabel(
        "El sistema arranca con la red vacía. Puedes cargar los archivos CSV individualmente "
        "o todos a la vez con \"Cargar todo\". El orden importa: sucursales → conexiones → productos.");
    info->setWordWrap(true);
    info->setStyleSheet(QString(
                            "color: %1; font-size: 12px; padding: 4px; background: %2;"
                            " border-radius: 4px; padding: 8px;"
                            ).arg(TEXTO_NEGRO).arg(GRIS_CLARO));
    cLay->addWidget(info);

    QHBoxLayout *btnLay = new QHBoxLayout();
    QPushButton *btnSuc  = new QPushButton("📂 Cargar Sucursales");
    QPushButton *btnConn = new QPushButton("📂 Cargar Conexiones");
    QPushButton *btnProd = new QPushButton("📂 Cargar Productos");
    QPushButton *btnTodo = new QPushButton("⚡ Cargar Todo (default)");
    QPushButton *btnLimpiar = new QPushButton("🗑 Limpiar Red");
    btnSuc->setStyleSheet(estiloBoton(AZUL_MEDIO));
    btnConn->setStyleSheet(estiloBoton(AZUL_MEDIO));
    btnProd->setStyleSheet(estiloBoton(AZUL_MEDIO));
    btnTodo->setStyleSheet(estiloBoton(VERDE));
    btnLimpiar->setStyleSheet(estiloBoton(ROJO));
    btnLay->addWidget(btnSuc);
    btnLay->addWidget(btnConn);
    btnLay->addWidget(btnProd);
    btnLay->addStretch();
    btnLay->addWidget(btnTodo);
    btnLay->addWidget(btnLimpiar);
    cLay->addLayout(btnLay);

    QLabel *lblLog = new QLabel("Esperando carga de archivos...");
    lblLog->setStyleSheet(QString(
                              "color: %1; font-family: 'Courier New'; font-size: 11px;"
                              " padding: 6px; background: %2; border: 1px solid %3; border-radius: 3px;"
                              ).arg(TEXTO_NEGRO).arg(GRIS_CLARO).arg(GRIS_BORDE));
    lblLog->setWordWrap(true);
    cLay->addWidget(lblLog);
    lay->addWidget(gbCarga);

    // ── Tabla de sucursales actualizable ──────────────────────
    QGroupBox *gbSuc = new QGroupBox("Inventario por sucursal");
    gbSuc->setStyleSheet(estiloGroupBox());
    QVBoxLayout *sucLay = new QVBoxLayout(gbSuc);
    sucLay->setContentsMargins(16, 16, 16, 16);

    QTableWidget *tbl = new QTableWidget();
    tbl->setColumnCount(5);
    tbl->setHorizontalHeaderLabels({"ID", "Nombre", "Ubicación",
                                    "Productos", "Tiempos (ing/tras/des)"});
    tbl->horizontalHeader()->setStretchLastSection(true);
    tbl->horizontalHeader()->setStyleSheet(estiloHeader());
    tbl->setSelectionBehavior(QAbstractItemView::SelectRows);
    tbl->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tbl->setAlternatingRowColors(true);
    tbl->setStyleSheet(estiloTabla());
    tbl->verticalHeader()->setVisible(false);
    sucLay->addWidget(tbl);
    lay->addWidget(gbSuc, 1);

    // Función para refrescar la tabla
    auto refrescarTabla = [=]() {
        auto sucursales = red->obtenerSucursales();
        tbl->setRowCount((int)sucursales.size());
        for (int i = 0; i < (int)sucursales.size(); i++) {
            Sucursal *s = sucursales[i];
            auto crearItem = [](const QString &t) {
                QTableWidgetItem *it = new QTableWidgetItem(t);
                it->setForeground(QColor(TEXTO_NEGRO));
                return it;
            };
            tbl->setItem(i, 0, crearItem(QString::fromStdString(s->getId())));
            tbl->setItem(i, 1, crearItem(QString::fromStdString(s->getNombre())));
            tbl->setItem(i, 2, crearItem(QString::fromStdString(s->getUbicacion())));
            tbl->setItem(i, 3, crearItem(QString::number(s->contarProductos())));
            tbl->setItem(i, 4, crearItem(
                                   QString("%1s / %2s / %3s")
                                       .arg(s->getTiempoIngreso())
                                       .arg(s->getTiempoTraspaso())
                                       .arg(s->getTiempoDespacho())));
        }
        tbl->resizeColumnsToContents();
    };
    refrescarTabla();

    // ── Conexiones de los botones de carga ────────────────────
    auto cargarSucursales = [=](const QString &ruta) {
        CargadorRed cargador("data/errors_red.log");
        int n = cargador.cargarSucursales(ruta.toStdString(), *red);
        lblLog->setText(QString("✔ %1 sucursales cargadas desde %2").arg(n).arg(ruta));
        refrescarTabla();
        actualizarTarjetas();
        emit redActualizada();
    };
    auto cargarConexiones = [=](const QString &ruta) {
        if (red->contarSucursales() == 0) {
            QMessageBox::warning(w, "Atención",
                                 "Carga primero las sucursales antes que las conexiones.");
            return;
        }
        CargadorRed cargador("data/errors_red.log");
        int n = cargador.cargarConexiones(ruta.toStdString(), *red);
        lblLog->setText(QString("✔ %1 conexiones cargadas desde %2").arg(n).arg(ruta));
        actualizarTarjetas();
        emit redActualizada();
    };
    auto cargarProductos = [=](const QString &ruta) {
        if (red->contarSucursales() == 0) {
            QMessageBox::warning(w, "Atención",
                                 "Carga primero las sucursales antes que los productos.");
            return;
        }
        CargadorRed cargador("data/errors_red.log");
        int n = cargador.cargarProductos(ruta.toStdString(), *red);
        lblLog->setText(QString("✔ %1 productos cargados desde %2").arg(n).arg(ruta));
        refrescarTabla();
        actualizarTarjetas();
        emit redActualizada();
    };

    connect(btnSuc, &QPushButton::clicked, [=]() {
        QString ruta = QFileDialog::getOpenFileName(w, "Cargar Sucursales",
                                                    "data/", "CSV (*.csv)");
        if (!ruta.isEmpty()) cargarSucursales(ruta);
    });
    connect(btnConn, &QPushButton::clicked, [=]() {
        QString ruta = QFileDialog::getOpenFileName(w, "Cargar Conexiones",
                                                    "data/", "CSV (*.csv)");
        if (!ruta.isEmpty()) cargarConexiones(ruta);
    });
    connect(btnProd, &QPushButton::clicked, [=]() {
        QString ruta = QFileDialog::getOpenFileName(w, "Cargar Productos",
                                                    "data/", "CSV (*.csv)");
        if (!ruta.isEmpty()) cargarProductos(ruta);
    });
    connect(btnTodo, &QPushButton::clicked, [=]() {
        if (red->contarSucursales() > 0) {
            int r = QMessageBox::question(w, "Confirmar",
                                          "La red ya tiene datos cargados. ¿Cargar los CSV adicionales?\n"
                                          "Los duplicados se omitirán.");
            if (r != QMessageBox::Yes) return;
        }
        CargadorRed cargador("data/errors_red.log");
        bool ok = cargador.cargarTodo(
            "data/sucursales.csv",
            "data/conexiones.csv",
            "data/productos_fase2.csv",
            *red);
        int total = 0;
        for (auto s : red->obtenerSucursales()) total += s->contarProductos();
        lblLog->setText(ok
                            ? QString("✔ Carga completa: %1 sucursales, %2 conexiones, %3 productos")
                                  .arg(red->contarSucursales())
                                  .arg(red->contarConexiones())
                                  .arg(total)
                            : "✗ Error en la carga. Revisa data/errors_red.log");
        refrescarTabla();
        actualizarTarjetas();
        emit redActualizada();
    });
    connect(btnLimpiar, &QPushButton::clicked, [=]() {
        int r = QMessageBox::question(w, "Confirmar",
                                      "¿Limpiar toda la red? Se perderán todos los datos no guardados.");
        if (r != QMessageBox::Yes) return;
        delete red;
        red = new RedSucursales();
        // Como el lambda captura `red` por copia del puntero, necesitamos
        // recrear el handler — pero como es miembro de la clase, usamos this->red
        // (este es el motivo por el cual `red` es miembro y no local)
        lblLog->setText("✔ Red limpiada. Lista para nueva carga.");
        refrescarTabla();
        actualizarTarjetas();
        emit redActualizada();
    });

    // Conectarse a redActualizada para refrescar la tabla automáticamente
    connect(this, &MainWindow::redActualizada, [=]() {
        refrescarTabla();
        actualizarTarjetas();
    });

    return w;
}

// ════════════════════════════════════════════════════════════
// TAB 2 — SUCURSALES (con auto-refresh)
// ════════════════════════════════════════════════════════════
QWidget* MainWindow::crearTabSucursales() {
    QWidget *w = new QWidget();
    w->setStyleSheet("background: white;");
    QHBoxLayout *lay = new QHBoxLayout(w);
    lay->setContentsMargins(12, 12, 12, 12);
    lay->setSpacing(12);

    // Panel izquierdo
    QGroupBox *gbLista = new QGroupBox("Sucursales en la red");
    gbLista->setStyleSheet(estiloGroupBox());
    gbLista->setFixedWidth(280);
    QVBoxLayout *lLay = new QVBoxLayout(gbLista);
    lLay->setContentsMargins(12, 16, 12, 12);

    QListWidget *listaSuc = new QListWidget();
    listaSuc->setStyleSheet(estiloLista());
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

    // Panel derecho
    QSplitter *splitter = new QSplitter(Qt::Vertical);

    QGroupBox *gbInfo = new QGroupBox("Detalle de sucursal");
    gbInfo->setStyleSheet(estiloGroupBox());
    QFormLayout *fLay = new QFormLayout(gbInfo);
    fLay->setSpacing(10);
    fLay->setContentsMargins(16, 20, 16, 16);

    QLineEdit *eId        = new QLineEdit(); eId->setReadOnly(true);
    QLineEdit *eNombre    = new QLineEdit();
    QLineEdit *eUbicacion = new QLineEdit();
    QSpinBox  *eTIngreso  = new QSpinBox(); eTIngreso->setRange(0, 9999); eTIngreso->setSuffix(" s");
    QSpinBox  *eTTraspaso = new QSpinBox(); eTTraspaso->setRange(0, 9999); eTTraspaso->setSuffix(" s");
    QSpinBox  *eTDespacho = new QSpinBox(); eTDespacho->setRange(0, 9999); eTDespacho->setSuffix(" s");

    QString cstyle = estiloCampo();
    eId->setStyleSheet(cstyle); eNombre->setStyleSheet(cstyle);
    eUbicacion->setStyleSheet(cstyle); eTIngreso->setStyleSheet(cstyle);
    eTTraspaso->setStyleSheet(cstyle); eTDespacho->setStyleSheet(cstyle);

    auto crearLabel = [](const QString &t) {
        QLabel *l = new QLabel(t);
        l->setStyleSheet(QString("color: %1; font-weight: bold;").arg(TEXTO_NEGRO));
        return l;
    };
    fLay->addRow(crearLabel("ID:"),                eId);
    fLay->addRow(crearLabel("Nombre:"),            eNombre);
    fLay->addRow(crearLabel("Ubicación:"),         eUbicacion);
    fLay->addRow(crearLabel("T. Ingreso (s):"),    eTIngreso);
    fLay->addRow(crearLabel("T. Traspaso (s):"),   eTTraspaso);
    fLay->addRow(crearLabel("T. Despacho (s):"),   eTDespacho);

    QPushButton *btnGuardar = new QPushButton("  Guardar cambios");
    btnGuardar->setStyleSheet(estiloBoton(AZUL_MEDIO));
    btnGuardar->setEnabled(false);
    fLay->addRow("", btnGuardar);
    splitter->addWidget(gbInfo);

    // Inventario
    QGroupBox *gbInv = new QGroupBox("Inventario de la sucursal");
    gbInv->setStyleSheet(estiloGroupBox());
    QVBoxLayout *invLay = new QVBoxLayout(gbInv);
    invLay->setContentsMargins(12, 16, 12, 12);

    QLineEdit *buscarInv = new QLineEdit();
    buscarInv->setPlaceholderText("Buscar producto en esta sucursal...");
    buscarInv->setStyleSheet(cstyle);
    invLay->addWidget(buscarInv);

    QTableWidget *tblInv = new QTableWidget();
    tblInv->setColumnCount(6);
    tblInv->setHorizontalHeaderLabels(
        {"Nombre", "Código", "Categoría", "Caducidad", "Precio Q", "Stock"});
    tblInv->horizontalHeader()->setStretchLastSection(true);
    tblInv->horizontalHeader()->setStyleSheet(estiloHeader());
    tblInv->setSelectionBehavior(QAbstractItemView::SelectRows);
    tblInv->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tblInv->setAlternatingRowColors(true);
    tblInv->setStyleSheet(estiloTabla());
    tblInv->verticalHeader()->setVisible(false);
    invLay->addWidget(tblInv);
    splitter->addWidget(gbInv);
    splitter->setSizes({240, 380});
    lay->addWidget(splitter, 1);

    // Función de refresh — clave para que los cambios en otros tabs se reflejen
    auto refrescarLista = [=]() {
        QString seleccionado;
        if (listaSuc->currentItem())
            seleccionado = listaSuc->currentItem()->text();
        listaSuc->clear();
        for (Sucursal *s : red->obtenerSucursales()) {
            listaSuc->addItem(
                QString("[%1] %2")
                    .arg(QString::fromStdString(s->getId()))
                    .arg(QString::fromStdString(s->getNombre())));
        }
        // Restaurar selección si todavía existe
        for (int i = 0; i < listaSuc->count(); i++) {
            if (listaSuc->item(i)->text() == seleccionado) {
                listaSuc->setCurrentRow(i); return;
            }
        }
        if (listaSuc->count() > 0) listaSuc->setCurrentRow(0);
        else {
            tblInv->setRowCount(0);
            eId->clear(); eNombre->clear(); eUbicacion->clear();
            eTIngreso->setValue(0); eTTraspaso->setValue(0); eTDespacho->setValue(0);
            btnGuardar->setEnabled(false);
        }
    };

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

        // Inventario via AVL in-order
        struct Walker {
            static void walk(NodoAVL *n, QTableWidget *t, int &r) {
                if (!n) return;
                walk(n->izquierda, t, r);
                Producto &p = n->dato;
                t->setRowCount(r + 1);
                auto mk = [](const QString &v) {
                    QTableWidgetItem *it = new QTableWidgetItem(v);
                    it->setForeground(QColor(TEXTO_NEGRO));
                    return it;
                };
                t->setItem(r, 0, mk(QString::fromStdString(p.nombre)));
                t->setItem(r, 1, mk(QString::fromStdString(p.codigoBarra)));
                t->setItem(r, 2, mk(QString::fromStdString(p.categoria)));
                t->setItem(r, 3, mk(QString::fromStdString(p.fechaCaducidad)));
                t->setItem(r, 4, mk(QString("Q%1").arg(p.precio, 0, 'f', 2)));
                t->setItem(r, 5, mk(QString::number(p.stock)));
                r++;
                walk(n->derecha, t, r);
            }
        };
        tblInv->setRowCount(0);
        int fila = 0;
        Walker::walk(s->getCatalogo()->obtenerArbolAVL()->obtenerRaiz(),
                     tblInv, fila);
        tblInv->resizeColumnsToContents();
        gbInv->setTitle(QString("Inventario — %1  (%2 productos)")
                            .arg(QString::fromStdString(s->getNombre()))
                            .arg(s->contarProductos()));
    };

    connect(listaSuc, &QListWidget::currentRowChanged, cargarSucursal);

    connect(buscarInv, &QLineEdit::textChanged, [=](const QString &t) {
        for (int i = 0; i < tblInv->rowCount(); i++) {
            bool vis = t.isEmpty() ||
                       tblInv->item(i, 0)->text().contains(t, Qt::CaseInsensitive) ||
                       tblInv->item(i, 1)->text().contains(t, Qt::CaseInsensitive) ||
                       tblInv->item(i, 2)->text().contains(t, Qt::CaseInsensitive);
            tblInv->setRowHidden(i, !vis);
        }
    });

    connect(btnGuardar, &QPushButton::clicked, [=]() {
        Sucursal *s = red->buscarSucursal(eId->text().toStdString());
        if (!s) return;
        s->setNombre(eNombre->text().toStdString());
        s->setUbicacion(eUbicacion->text().toStdString());
        s->setTiempoIngreso(eTIngreso->value());
        s->setTiempoTraspaso(eTTraspaso->value());
        s->setTiempoDespacho(eTDespacho->value());
        QMessageBox::information(w, "Guardado", "Sucursal actualizada.");
        emit redActualizada();
    });

    connect(btnAgregar, &QPushButton::clicked, [=]() {
        QDialog dlg(w);
        dlg.setStyleSheet("QDialog { background: white; } "
                          "QLabel { color: " + TEXTO_NEGRO + "; }");
        dlg.setWindowTitle("Nueva Sucursal");
        dlg.setFixedWidth(380);
        QFormLayout *fl = new QFormLayout(&dlg);
        fl->setContentsMargins(20, 20, 20, 20);
        fl->setSpacing(10);

        QLineEdit *dId  = new QLineEdit(); dId->setMaxLength(10);
        QLineEdit *dNom = new QLineEdit();
        QLineEdit *dUbi = new QLineEdit();
        QSpinBox  *dTI  = new QSpinBox(); dTI->setRange(0, 9999); dTI->setValue(30); dTI->setSuffix(" s");
        QSpinBox  *dTT  = new QSpinBox(); dTT->setRange(0, 9999); dTT->setValue(45); dTT->setSuffix(" s");
        QSpinBox  *dTD  = new QSpinBox(); dTD->setRange(0, 9999); dTD->setValue(20); dTD->setSuffix(" s");
        dId->setStyleSheet(cstyle); dNom->setStyleSheet(cstyle);
        dUbi->setStyleSheet(cstyle); dTI->setStyleSheet(cstyle);
        dTT->setStyleSheet(cstyle); dTD->setStyleSheet(cstyle);

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
                dId->text().toStdString(), dNom->text().toStdString(),
                dUbi->text().toStdString(),
                dTI->value(), dTT->value(), dTD->value());
            if (red->agregarSucursal(ns)) {
                emit redActualizada(); // dispara refresh en TODOS los tabs
            } else {
                delete ns;
                QMessageBox::warning(w, "Error", "Ya existe una sucursal con ese ID.");
            }
        }
    });

    connect(btnEliminar, &QPushButton::clicked, [=]() {
        int row = listaSuc->currentRow();
        if (row < 0) {
            QMessageBox::information(w, "Info", "Selecciona una sucursal."); return;
        }
        auto suc = red->obtenerSucursales()[row];
        int r = QMessageBox::question(w, "Confirmar",
                                      QString("¿Eliminar '%1'? Se perderá su inventario y conexiones.")
                                          .arg(QString::fromStdString(suc->getNombre())));
        if (r == QMessageBox::Yes) {
            red->eliminarSucursal(suc->getId());
            emit redActualizada();
        }
    });

    // Auto-refresh cuando la red cambie (¡Aquí el fix del problema 3!)
    connect(this, &MainWindow::redActualizada, refrescarLista);
    refrescarLista();

    return w;
}

// ════════════════════════════════════════════════════════════
// TAB 3 — RED (con auto-refresh)
// ════════════════════════════════════════════════════════════
QWidget* MainWindow::crearTabRed() {
    QWidget *w = new QWidget();
    w->setStyleSheet("background: white;");
    QHBoxLayout *lay = new QHBoxLayout(w);
    lay->setContentsMargins(12, 12, 12, 12);
    lay->setSpacing(12);

    QWidget *panel = new QWidget();
    panel->setFixedWidth(260);
    QVBoxLayout *pLay = new QVBoxLayout(panel);
    pLay->setSpacing(10);

    QGroupBox *gbCtrl = new QGroupBox("Controles");
    gbCtrl->setStyleSheet(estiloGroupBox());
    QVBoxLayout *cLay = new QVBoxLayout(gbCtrl);
    cLay->setContentsMargins(12, 16, 12, 12);

    QPushButton *btnRedibujar  = new QPushButton("↺  Redibujar grafo");
    QPushButton *btnAgregarConn = new QPushButton("+  Nueva conexión");
    QPushButton *btnElimConn   = new QPushButton("✕  Eliminar conexión");
    btnRedibujar->setStyleSheet(estiloBoton(AZUL_MEDIO));
    btnAgregarConn->setStyleSheet(estiloBoton(VERDE));
    btnElimConn->setStyleSheet(estiloBoton(ROJO));
    cLay->addWidget(btnRedibujar);
    cLay->addWidget(btnAgregarConn);
    cLay->addWidget(btnElimConn);
    pLay->addWidget(gbCtrl);

    QGroupBox *gbConns = new QGroupBox("Conexiones");
    gbConns->setStyleSheet(estiloGroupBox());
    QVBoxLayout *connLay = new QVBoxLayout(gbConns);
    connLay->setContentsMargins(12, 16, 12, 12);
    QListWidget *listaConns = new QListWidget();
    listaConns->setStyleSheet(estiloLista());
    connLay->addWidget(listaConns);
    pLay->addWidget(gbConns, 1);
    lay->addWidget(panel);

    QGraphicsScene *scene = new QGraphicsScene();
    scene->setBackgroundBrush(Qt::white);
    QGraphicsView *view = new QGraphicsView(scene);
    view->setRenderHint(QPainter::Antialiasing);
    view->setDragMode(QGraphicsView::ScrollHandDrag);
    view->setStyleSheet(QString("border: 1px solid %1; background: white;").arg(GRIS_BORDE));

    auto refrescarLista = [=]() {
        listaConns->clear();
        for (const Conexion &c : red->obtenerConexiones()) {
            listaConns->addItem(
                QString("%1 → %2  t=%3s  Q%4")
                    .arg(QString::fromStdString(c.origenId))
                    .arg(QString::fromStdString(c.destinoId))
                    .arg(c.tiempo).arg(c.costo));
        }
    };

    auto dibujarGrafo = [=]() {
        scene->clear();
        auto sucursales = red->obtenerSucursales();
        int n = (int)sucursales.size();
        if (n == 0) {
            QGraphicsTextItem *t = scene->addText(
                "Red vacía. Carga sucursales desde el Tab Sistema.");
            t->setFont(QFont("Arial", 12));
            t->setDefaultTextColor(QColor("#888"));
            return;
        }

        const double R = 240.0;
        const double cx = 320.0, cy = 280.0;
        QMap<QString, QPointF> pos;
        for (int i = 0; i < n; i++) {
            double ang = 2.0 * M_PI * i / n - M_PI / 2.0;
            pos[QString::fromStdString(sucursales[i]->getId())] =
                QPointF(cx + R * std::cos(ang), cy + R * std::sin(ang));
        }

        // Aristas
        for (const Conexion &c : red->obtenerConexiones()) {
            QPointF p1 = pos[QString::fromStdString(c.origenId)];
            QPointF p2 = pos[QString::fromStdString(c.destinoId)];
            scene->addLine(p1.x(), p1.y(), p2.x(), p2.y(),
                           QPen(QColor("#90A4AE"), 2));
            QPointF mid = (p1 + p2) / 2.0;
            QGraphicsTextItem *etq = scene->addText(
                QString("t=%1s  Q%2").arg(c.tiempo).arg(c.costo));
            etq->setFont(QFont("Arial", 8));
            etq->setDefaultTextColor(QColor("#37474F"));
            etq->setPos(mid.x() - 30, mid.y() - 12);
        }

        // Nodos
        const double Rn = 36.0;
        for (Sucursal *s : sucursales) {
            QPointF p = pos[QString::fromStdString(s->getId())];
            scene->addEllipse(p.x() - Rn, p.y() - Rn, Rn * 2, Rn * 2,
                              QPen(QColor(AZUL_OSCURO), 2), QBrush(QColor(AZUL_CLARO)));

            QGraphicsTextItem *ti = scene->addText(QString::fromStdString(s->getId()));
            ti->setFont(QFont("Arial", 10, QFont::Bold));
            ti->setDefaultTextColor(QColor(AZUL_OSCURO));
            ti->setPos(p.x() - ti->boundingRect().width()/2,
                       p.y() - ti->boundingRect().height()/2 - 5);

            QGraphicsTextItem *tp = scene->addText(
                QString("%1 prods").arg(s->contarProductos()));
            tp->setFont(QFont("Arial", 8));
            tp->setDefaultTextColor(QColor("#37474F"));
            tp->setPos(p.x() - tp->boundingRect().width()/2, p.y() + 6);

            QGraphicsTextItem *tn = scene->addText(QString::fromStdString(s->getNombre()));
            tn->setFont(QFont("Arial", 9));
            tn->setDefaultTextColor(QColor("#263238"));
            tn->setPos(p.x() - tn->boundingRect().width()/2, p.y() + Rn + 4);
        }
        view->fitInView(scene->sceneRect().adjusted(-40, -40, 40, 40),
                        Qt::KeepAspectRatio);
    };

    connect(btnRedibujar, &QPushButton::clicked, dibujarGrafo);

    connect(btnAgregarConn, &QPushButton::clicked, [=]() {
        if (red->contarSucursales() < 2) {
            QMessageBox::warning(w, "Atención",
                                 "Necesitas al menos 2 sucursales para crear una conexión.");
            return;
        }
        QDialog dlg(w);
        dlg.setStyleSheet("QDialog { background: white; } "
                          "QLabel { color: " + TEXTO_NEGRO + "; }");
        dlg.setWindowTitle("Nueva Conexión");
        dlg.setFixedWidth(360);
        QFormLayout *fl = new QFormLayout(&dlg);
        fl->setContentsMargins(20, 20, 20, 20);

        QString cstyle = estiloCampo();
        QComboBox *cOrigen = new QComboBox();
        QComboBox *cDestino = new QComboBox();
        for (Sucursal *s : red->obtenerSucursales()) {
            QString lbl = QString::fromStdString(s->getId() + " — " + s->getNombre());
            cOrigen->addItem(lbl, QString::fromStdString(s->getId()));
            cDestino->addItem(lbl, QString::fromStdString(s->getId()));
        }
        if (cDestino->count() > 1) cDestino->setCurrentIndex(1);
        cOrigen->setStyleSheet(cstyle); cDestino->setStyleSheet(cstyle);

        QDoubleSpinBox *dT = new QDoubleSpinBox();
        dT->setRange(1, 9999); dT->setValue(30); dT->setSuffix(" s");
        QDoubleSpinBox *dC = new QDoubleSpinBox();
        dC->setRange(0.01, 9999); dC->setValue(15); dC->setPrefix("Q ");
        dT->setStyleSheet(cstyle); dC->setStyleSheet(cstyle);

        QCheckBox *chkBi = new QCheckBox("Bidireccional");
        chkBi->setChecked(true);
        chkBi->setStyleSheet(QString("color: %1;").arg(TEXTO_NEGRO));

        fl->addRow("Origen:",   cOrigen);
        fl->addRow("Destino:",  cDestino);
        fl->addRow("Tiempo:",   dT);
        fl->addRow("Costo:",    dC);
        fl->addRow("",          chkBi);

        QDialogButtonBox *bb = new QDialogButtonBox(
            QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
        fl->addRow(bb);
        connect(bb, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
        connect(bb, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);

        if (dlg.exec() == QDialog::Accepted) {
            QString o = cOrigen->currentData().toString();
            QString d = cDestino->currentData().toString();
            if (o == d) { QMessageBox::warning(w, "Error",
                                     "Origen y destino deben ser distintos."); return; }
            if (red->agregarConexion(o.toStdString(), d.toStdString(),
                                     dT->value(), dC->value(), chkBi->isChecked())) {
                emit redActualizada();
            }
        }
    });

    connect(btnElimConn, &QPushButton::clicked, [=]() {
        int row = listaConns->currentRow();
        if (row < 0) {
            QMessageBox::information(w, "Info", "Selecciona una conexión."); return;
        }
        QString texto = listaConns->item(row)->text();
        QString o = texto.split(" → ")[0].trimmed();
        QString d = texto.split(" → ")[1].split("  ")[0].trimmed();
        if (red->eliminarConexion(o.toStdString(), d.toStdString())) {
            emit redActualizada();
        }
    });

    lay->addWidget(view, 1);

    // Auto-refresh
    connect(this, &MainWindow::redActualizada, [=]() {
        refrescarLista();
        dibujarGrafo();
    });
    refrescarLista();
    QTimer::singleShot(150, dibujarGrafo);
    return w;
}

// ════════════════════════════════════════════════════════════
// TAB 4 — INVENTARIO (con auto-refresh)
// ════════════════════════════════════════════════════════════
QWidget* MainWindow::crearTabInventario() {
    QWidget *w = new QWidget();
    w->setStyleSheet("background: white;");
    QVBoxLayout *lay = new QVBoxLayout(w);
    lay->setContentsMargins(12, 12, 12, 12);
    lay->setSpacing(10);

    QString cstyle = estiloCampo();

    QHBoxLayout *buscLay = new QHBoxLayout();
    QComboBox *cmbSucursal = new QComboBox();
    cmbSucursal->setFixedWidth(220);
    cmbSucursal->setStyleSheet(cstyle);
    QLineEdit *buscar = new QLineEdit();
    buscar->setPlaceholderText("Nombre, código, categoría o fechas (YYYY-MM-DD / YYYY-MM-DD)...");
    buscar->setStyleSheet(cstyle);
    QComboBox *cmbCriterio = new QComboBox();
    cmbCriterio->addItems({"Por nombre (AVL)",
                           "Por código (Hash)",
                           "Por categoría (B+)",
                           "Por rango de fecha (B)"});
    cmbCriterio->setFixedWidth(220);
    cmbCriterio->setStyleSheet(cstyle);

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

    QTableWidget *tbl = new QTableWidget();
    tbl->setColumnCount(8);
    tbl->setHorizontalHeaderLabels(
        {"Sucursal", "Nombre", "Código", "Categoría",
         "Caducidad", "Marca", "Precio Q", "Stock"});
    tbl->horizontalHeader()->setStretchLastSection(true);
    tbl->horizontalHeader()->setStyleSheet(estiloHeader());
    tbl->setSelectionBehavior(QAbstractItemView::SelectRows);
    tbl->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tbl->setAlternatingRowColors(true);
    tbl->setStyleSheet(estiloTabla());
    tbl->verticalHeader()->setVisible(false);
    lay->addWidget(tbl, 1);

    QLabel *lblConteo = new QLabel("Sin productos");
    lblConteo->setStyleSheet(QString("color: %1; font-size: 11px;").arg(TEXTO_NEGRO));
    lay->addWidget(lblConteo);

    auto refrescarCombo = [=]() {
        QString prev = cmbSucursal->currentData().toString();
        cmbSucursal->clear();
        cmbSucursal->addItem("— Todas las sucursales —", "ALL");
        for (Sucursal *s : red->obtenerSucursales())
            cmbSucursal->addItem(
                QString::fromStdString(s->getId() + " — " + s->getNombre()),
                QString::fromStdString(s->getId()));
        if (!prev.isEmpty()) {
            for (int i = 0; i < cmbSucursal->count(); i++) {
                if (cmbSucursal->itemData(i).toString() == prev) {
                    cmbSucursal->setCurrentIndex(i); break;
                }
            }
        }
    };

    auto cargarTodos = [=]() {
        tbl->setRowCount(0);
        QString filtro = cmbSucursal->currentData().toString();
        int fila = 0;

        struct Walker {
            static void walk(NodoAVL *n, QTableWidget *t, int &r,
                             const std::string &sucId, const QString &filtro) {
                if (!n) return;
                walk(n->izquierda, t, r, sucId, filtro);
                Producto &p = n->dato;
                if (filtro == "ALL" ||
                    QString::fromStdString(p.sucursalId) == filtro) {
                    auto mk = [](const QString &v) {
                        QTableWidgetItem *it = new QTableWidgetItem(v);
                        it->setForeground(QColor(TEXTO_NEGRO));
                        return it;
                    };
                    t->setRowCount(r + 1);
                    t->setItem(r, 0, mk(QString::fromStdString(p.sucursalId)));
                    t->setItem(r, 1, mk(QString::fromStdString(p.nombre)));
                    t->setItem(r, 2, mk(QString::fromStdString(p.codigoBarra)));
                    t->setItem(r, 3, mk(QString::fromStdString(p.categoria)));
                    t->setItem(r, 4, mk(QString::fromStdString(p.fechaCaducidad)));
                    t->setItem(r, 5, mk(QString::fromStdString(p.marca)));
                    t->setItem(r, 6, mk(QString("Q%1").arg(p.precio, 0, 'f', 2)));
                    t->setItem(r, 7, mk(QString::number(p.stock)));
                    r++;
                }
                walk(n->derecha, t, r, sucId, filtro);
            }
        };

        for (Sucursal *s : red->obtenerSucursales()) {
            if (filtro != "ALL" &&
                QString::fromStdString(s->getId()) != filtro) continue;
            Walker::walk(s->getCatalogo()->obtenerArbolAVL()->obtenerRaiz(),
                         tbl, fila, s->getId(), filtro);
        }
        tbl->resizeColumnsToContents();
        lblConteo->setText(QString("Mostrando %1 productos").arg(tbl->rowCount()));
    };

    connect(cmbSucursal, QOverload<int>::of(&QComboBox::currentIndexChanged),
            [=](int) { cargarTodos(); });

    // Helper para agregar una fila a la tabla de resultados
    auto agregarFila = [=](int &fila, const QString &sucId, Producto *p) {
        if (!p) return;
        auto mk = [](const QString &v) {
            QTableWidgetItem *it = new QTableWidgetItem(v);
            it->setForeground(QColor(TEXTO_NEGRO));
            return it;
        };
        tbl->setRowCount(fila + 1);
        tbl->setItem(fila, 0, mk(sucId));
        tbl->setItem(fila, 1, mk(QString::fromStdString(p->nombre)));
        tbl->setItem(fila, 2, mk(QString::fromStdString(p->codigoBarra)));
        tbl->setItem(fila, 3, mk(QString::fromStdString(p->categoria)));
        tbl->setItem(fila, 4, mk(QString::fromStdString(p->fechaCaducidad)));
        tbl->setItem(fila, 5, mk(QString::fromStdString(p->marca)));
        tbl->setItem(fila, 6, mk(QString("Q%1").arg(p->precio, 0, 'f', 2)));
        tbl->setItem(fila, 7, mk(QString::number(p->stock)));
        fila++;
    };

    connect(btnBuscar, &QPushButton::clicked, [=]() {
        QString texto = buscar->text().trimmed();
        if (texto.isEmpty()) { cargarTodos(); return; }

        int     crit   = cmbCriterio->currentIndex();
        QString filtro = cmbSucursal->currentData().toString();
        tbl->setRowCount(0);
        int fila = 0;

        // ── Criterio 0: Nombre exacto — AVL O(log n) ─────────
        if (crit == 0) {
            for (Sucursal *s : red->obtenerSucursales()) {
                if (filtro != "ALL" &&
                    QString::fromStdString(s->getId()) != filtro) continue;
                Producto *p = s->getCatalogo()->buscarPorNombre(texto.toStdString());
                agregarFila(fila, QString::fromStdString(s->getId()), p);
            }
        }
        // ── Criterio 1: Código de barra — Hash O(1) ──────────
        else if (crit == 1) {
            for (Sucursal *s : red->obtenerSucursales()) {
                if (filtro != "ALL" &&
                    QString::fromStdString(s->getId()) != filtro) continue;
                Producto *p = s->getCatalogo()->buscarPorCodigo(texto.toStdString());
                agregarFila(fila, QString::fromStdString(s->getId()), p);
            }
        }
        // ── Criterio 2: Categoría — Árbol B+ O(log n + k) ────
        else if (crit == 2) {
            // El B+ devuelve múltiples productos — los recogemos via Walker en AVL
            // filtrando por categoría (más directo que capturar stdout del B+)
            struct CatWalker {
                static void walk(NodoAVL *n, const std::string &cat,
                                 QTableWidget *t, int &r,
                                 const QString &sucId) {
                    if (!n) return;
                    walk(n->izquierda, cat, t, r, sucId);
                    if (n->dato.categoria == cat) {
                        auto mk = [](const QString &v) {
                            QTableWidgetItem *it = new QTableWidgetItem(v);
                            it->setForeground(QColor(TEXTO_NEGRO));
                            return it;
                        };
                        Producto &p = n->dato;
                        t->setRowCount(r + 1);
                        t->setItem(r, 0, mk(sucId));
                        t->setItem(r, 1, mk(QString::fromStdString(p.nombre)));
                        t->setItem(r, 2, mk(QString::fromStdString(p.codigoBarra)));
                        t->setItem(r, 3, mk(QString::fromStdString(p.categoria)));
                        t->setItem(r, 4, mk(QString::fromStdString(p.fechaCaducidad)));
                        t->setItem(r, 5, mk(QString::fromStdString(p.marca)));
                        t->setItem(r, 6, mk(QString("Q%1").arg(p.precio, 0, 'f', 2)));
                        t->setItem(r, 7, mk(QString::number(p.stock)));
                        r++;
                    }
                    walk(n->derecha, cat, t, r, sucId);
                }
            };
            for (Sucursal *s : red->obtenerSucursales()) {
                if (filtro != "ALL" &&
                    QString::fromStdString(s->getId()) != filtro) continue;
                CatWalker::walk(
                    s->getCatalogo()->obtenerArbolAVL()->obtenerRaiz(),
                    texto.toStdString(), tbl, fila,
                    QString::fromStdString(s->getId()));
            }
        }
        // ── Criterio 3: Rango de fechas — Árbol B O(log n + k)
        else if (crit == 3) {
            // Formato esperado: "YYYY-MM-DD / YYYY-MM-DD"
            QStringList partes = texto.split("/");
            if (partes.size() != 2) {
                lblConteo->setText(
                    "[!] Formato: YYYY-MM-DD / YYYY-MM-DD");
                return;
            }
            QString ini = partes[0].trimmed();
            QString fin = partes[1].trimmed();
            if (ini > fin) {
                lblConteo->setText("[!] La fecha inicio debe ser ≤ fecha fin.");
                return;
            }
            // Recorremos el AVL filtrando por rango de fechas
            struct FechaWalker {
                static void walk(NodoAVL *n, const std::string &ini,
                                 const std::string &fin,
                                 QTableWidget *t, int &r,
                                 const QString &sucId) {
                    if (!n) return;
                    walk(n->izquierda, ini, fin, t, r, sucId);
                    if (n->dato.fechaCaducidad >= ini &&
                        n->dato.fechaCaducidad <= fin) {
                        auto mk = [](const QString &v) {
                            QTableWidgetItem *it = new QTableWidgetItem(v);
                            it->setForeground(QColor(TEXTO_NEGRO));
                            return it;
                        };
                        Producto &p = n->dato;
                        t->setRowCount(r + 1);
                        t->setItem(r, 0, mk(sucId));
                        t->setItem(r, 1, mk(QString::fromStdString(p.nombre)));
                        t->setItem(r, 2, mk(QString::fromStdString(p.codigoBarra)));
                        t->setItem(r, 3, mk(QString::fromStdString(p.categoria)));
                        t->setItem(r, 4, mk(QString::fromStdString(p.fechaCaducidad)));
                        t->setItem(r, 5, mk(QString::fromStdString(p.marca)));
                        t->setItem(r, 6, mk(QString("Q%1").arg(p.precio, 0, 'f', 2)));
                        t->setItem(r, 7, mk(QString::number(p.stock)));
                        r++;
                    }
                    walk(n->derecha, ini, fin, t, r, sucId);
                }
            };
            for (Sucursal *s : red->obtenerSucursales()) {
                if (filtro != "ALL" &&
                    QString::fromStdString(s->getId()) != filtro) continue;
                FechaWalker::walk(
                    s->getCatalogo()->obtenerArbolAVL()->obtenerRaiz(),
                    ini.toStdString(), fin.toStdString(),
                    tbl, fila, QString::fromStdString(s->getId()));
            }
        }

        tbl->resizeColumnsToContents();
        lblConteo->setText(fila == 0
                               ? "Sin resultados"
                               : QString("%1 resultado(s) — estructura: %2").arg(fila)
                                     .arg(QStringList{"AVL","Hash","B+","B"}[crit]));
    });

    connect(buscar, &QLineEdit::returnPressed, btnBuscar, &QPushButton::click);

    connect(btnAgregar, &QPushButton::clicked, [=]() {
        if (red->contarSucursales() == 0) {
            QMessageBox::warning(w, "Atención",
                                 "Crea al menos una sucursal antes de agregar productos.");
            return;
        }
        QDialog dlg(w);
        dlg.setStyleSheet("QDialog { background: white; } "
                          "QLabel { color: " + TEXTO_NEGRO + "; }");
        dlg.setWindowTitle("Agregar Producto");
        dlg.setFixedWidth(420);
        QFormLayout *fl = new QFormLayout(&dlg);
        fl->setContentsMargins(20, 20, 20, 20);
        fl->setSpacing(10);

        QString cs = estiloCampo();
        QComboBox *dSuc = new QComboBox();
        for (Sucursal *s : red->obtenerSucursales())
            dSuc->addItem(QString::fromStdString(s->getId() + " — " + s->getNombre()),
                          QString::fromStdString(s->getId()));
        QString cur = cmbSucursal->currentData().toString();
        if (cur != "ALL") {
            for (int i = 0; i < dSuc->count(); i++) {
                if (dSuc->itemData(i).toString() == cur) {
                    dSuc->setCurrentIndex(i); break;
                }
            }
        }
        QLineEdit *dNom = new QLineEdit();
        QLineEdit *dCod = new QLineEdit(); dCod->setMaxLength(10);
        dCod->setPlaceholderText("Exactamente 10 dígitos");
        QLineEdit *dCat = new QLineEdit();
        QLineEdit *dFec = new QLineEdit(); dFec->setPlaceholderText("YYYY-MM-DD");
        QLineEdit *dMar = new QLineEdit();
        QDoubleSpinBox *dPre = new QDoubleSpinBox();
        dPre->setRange(0, 99999); dPre->setPrefix("Q "); dPre->setDecimals(2);
        QSpinBox *dSto = new QSpinBox(); dSto->setRange(0, 99999);

        for (auto *wid : {(QWidget*)dSuc, (QWidget*)dNom, (QWidget*)dCod,
                          (QWidget*)dCat, (QWidget*)dFec, (QWidget*)dMar,
                          (QWidget*)dPre, (QWidget*)dSto})
            wid->setStyleSheet(cs);

        fl->addRow("Sucursal:",     dSuc);
        fl->addRow("Nombre:",       dNom);
        fl->addRow("Código (10):",  dCod);
        fl->addRow("Categoría:",    dCat);
        fl->addRow("Caducidad:",    dFec);
        fl->addRow("Marca:",        dMar);
        fl->addRow("Precio:",       dPre);
        fl->addRow("Stock:",        dSto);

        QDialogButtonBox *bb = new QDialogButtonBox(
            QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
        fl->addRow(bb);
        connect(bb, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
        connect(bb, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);

        if (dlg.exec() == QDialog::Accepted) {
            if (dCod->text().length() != 10) {
                QMessageBox::warning(w, "Error",
                                     "El código debe tener exactamente 10 dígitos.");
                return;
            }
            QString sucId = dSuc->currentData().toString();
            Sucursal *s = red->buscarSucursal(sucId.toStdString());
            if (!s) return;
            Producto p(dNom->text().toStdString(), dCod->text().toStdString(),
                       dCat->text().toStdString(), dFec->text().toStdString(),
                       dMar->text().toStdString(),
                       dPre->value(), dSto->value(), sucId.toStdString());
            if (s->agregarProducto(p)) {
                emit redActualizada();
                QMessageBox::information(w, "OK",
                                         QString("Producto '%1' agregado.").arg(dNom->text()));
            } else {
                QMessageBox::warning(w, "Error", "No se pudo agregar el producto.");
            }
        }
    });

    connect(btnEliminar, &QPushButton::clicked, [=]() {
        int row = tbl->currentRow();
        if (row < 0) {
            QMessageBox::information(w, "Info", "Selecciona un producto."); return;
        }
        QString sucId  = tbl->item(row, 0)->text();
        QString nombre = tbl->item(row, 1)->text();
        QString codigo = tbl->item(row, 2)->text();
        QString cat    = tbl->item(row, 3)->text();
        QString fecha  = tbl->item(row, 4)->text();

        int r = QMessageBox::question(w, "Confirmar",
                                      QString("¿Eliminar '%1' de %2?").arg(nombre).arg(sucId));
        if (r == QMessageBox::Yes) {
            Sucursal *s = red->buscarSucursal(sucId.toStdString());
            if (s && s->eliminarProducto(nombre.toStdString(), codigo.toStdString(),
                                         cat.toStdString(), fecha.toStdString())) {
                emit redActualizada();
            }
        }
    });

    // Auto-refresh
    connect(this, &MainWindow::redActualizada, [=]() {
        refrescarCombo();
        cargarTodos();
    });
    refrescarCombo();
    cargarTodos();
    return w;
}

// ════════════════════════════════════════════════════════════
// TAB 5 — TRANSFERENCIA (con auto-refresh de combos)
// ════════════════════════════════════════════════════════════
QWidget* MainWindow::crearTabTransferencia() {
    QWidget *w = new QWidget();
    w->setStyleSheet("background: white;");
    QHBoxLayout *lay = new QHBoxLayout(w);
    lay->setContentsMargins(12, 12, 12, 12);
    lay->setSpacing(12);

    QString cstyle = estiloCampo();

    // Panel izquierdo dentro de QScrollArea para que no se comprima
    QScrollArea *scrollPanel = new QScrollArea();
    scrollPanel->setFixedWidth(400);
    scrollPanel->setWidgetResizable(true);
    scrollPanel->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollPanel->setStyleSheet(QString(
                                   "QScrollArea { border: none; background: white; }"
                                   "QScrollBar:vertical { width: 6px; background: %1; }"
                                   "QScrollBar::handle:vertical { background: %2; border-radius: 3px; }"
                                   ).arg(GRIS_CLARO).arg(AZUL_CLARO));

    QWidget *panel = new QWidget();
    panel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
    QVBoxLayout *pLay = new QVBoxLayout(panel);
    pLay->setSpacing(10);
    pLay->setContentsMargins(4, 4, 4, 4);

    QGroupBox *gbConf = new QGroupBox("Configurar transferencia");
    gbConf->setStyleSheet(estiloGroupBox());
    QFormLayout *fLay = new QFormLayout(gbConf);
    fLay->setSpacing(8);
    fLay->setContentsMargins(12, 14, 12, 14);
    fLay->setLabelAlignment(Qt::AlignRight | Qt::AlignVCenter);
    fLay->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);

    QComboBox *cOrigen  = new QComboBox(); cOrigen->setStyleSheet(cstyle);
    QComboBox *cDestino = new QComboBox(); cDestino->setStyleSheet(cstyle);

    QLineEdit *eCodigo = new QLineEdit();
    eCodigo->setPlaceholderText("Código de barra (10 dígitos)");
    eCodigo->setStyleSheet(cstyle);

    QSpinBox *eUnidades = new QSpinBox();
    eUnidades->setRange(1, 999999);
    eUnidades->setValue(1);
    eUnidades->setSpecialValueText("Todo el stock (1)");
    eUnidades->setStyleSheet(cstyle);

    QRadioButton *rTiempo = new QRadioButton("Minimizar tiempo");
    QRadioButton *rCosto  = new QRadioButton("Minimizar costo");
    rTiempo->setChecked(true);
    rTiempo->setStyleSheet(QString("color: %1;").arg(TEXTO_NEGRO));
    rCosto->setStyleSheet(QString("color: %1;").arg(TEXTO_NEGRO));
    QButtonGroup *bgCriterio = new QButtonGroup(w);
    bgCriterio->addButton(rTiempo, 0);
    bgCriterio->addButton(rCosto, 1);
    QHBoxLayout *rhLay = new QHBoxLayout();
    rhLay->addWidget(rTiempo); rhLay->addWidget(rCosto);

    // Selector de velocidad de simulación
    QComboBox *cmbVelocidad = new QComboBox();
    cmbVelocidad->addItem("1x — tiempo real (lento)", 1);
    cmbVelocidad->addItem("10x — moderado",            10);
    cmbVelocidad->addItem("30x — recomendado",         30);
    cmbVelocidad->addItem("60x — rápido",              60);
    cmbVelocidad->addItem("120x — muy rápido",         120);
    cmbVelocidad->setCurrentIndex(2); // 30x default
    cmbVelocidad->setStyleSheet(cstyle);

    auto mkLabel = [](const QString &t) {
        QLabel *l = new QLabel(t);
        l->setMinimumWidth(72);
        l->setStyleSheet(QString("color: %1; font-weight: bold; font-size: 12px;")
                             .arg(TEXTO_NEGRO));
        return l;
    };
    fLay->addRow(mkLabel("Origen:"),    cOrigen);
    fLay->addRow(mkLabel("Destino:"),   cDestino);

    // Separador visual entre destino y datos del producto
    QFrame *sep1 = new QFrame();
    sep1->setFrameShape(QFrame::HLine);
    sep1->setStyleSheet(QString("color: %1;").arg(GRIS_BORDE));
    fLay->addRow(sep1);

    fLay->addRow(mkLabel("Código:"),    eCodigo);
    fLay->addRow(mkLabel("Unidades:"),  eUnidades);

    // Separador visual antes de opciones
    QFrame *sep2 = new QFrame();
    sep2->setFrameShape(QFrame::HLine);
    sep2->setStyleSheet(QString("color: %1;").arg(GRIS_BORDE));
    fLay->addRow(sep2);

    fLay->addRow(mkLabel("Criterio:"),  rhLay);
    fLay->addRow(mkLabel("Velocidad:"), cmbVelocidad);

    QPushButton *btnCalc   = new QPushButton("⟶  Calcular ruta óptima");
    QPushButton *btnTrans  = new QPushButton("▶  Simular transferencia");
    QPushButton *btnCancel = new QPushButton("✕  Cancelar simulación");
    btnCalc->setStyleSheet(estiloBoton(AZUL_MEDIO));
    btnTrans->setStyleSheet(estiloBoton(VERDE));
    btnCancel->setStyleSheet(estiloBoton(ROJO));
    btnTrans->setEnabled(false);
    btnCancel->setEnabled(false);
    // Separador antes de botones
    QFrame *sep3 = new QFrame();
    sep3->setFrameShape(QFrame::HLine);
    sep3->setStyleSheet(QString("color: %1;").arg(GRIS_BORDE));
    fLay->addRow(sep3);

    btnCalc->setMinimumHeight(36);
    btnTrans->setMinimumHeight(36);
    btnCancel->setMinimumHeight(36);
    fLay->addRow("", btnCalc);
    fLay->addRow("", btnTrans);
    fLay->addRow("", btnCancel);
    pLay->addWidget(gbConf);

    // Panel del estado actual de la simulación
    QGroupBox *gbEstado = new QGroupBox("Estado de la simulación");
    gbEstado->setStyleSheet(estiloGroupBox());
    QVBoxLayout *estLay = new QVBoxLayout(gbEstado);
    estLay->setContentsMargins(12, 16, 12, 12);

    // Badge: estado del producto
    QLabel *lblEstadoProd = new QLabel("Sin simulación activa");
    lblEstadoProd->setAlignment(Qt::AlignCenter);
    lblEstadoProd->setStyleSheet(QString(
                                     "background: %1; color: white; font-weight: bold;"
                                     " padding: 8px; border-radius: 4px; font-size: 13px;"
                                     ).arg("#888"));
    estLay->addWidget(lblEstadoProd);

    // Sucursal actual
    QLabel *lblSucActual = new QLabel("Sucursal actual: —");
    lblSucActual->setStyleSheet(QString(
                                    "color: %1; font-size: 12px; padding: 4px;").arg(TEXTO_NEGRO));
    estLay->addWidget(lblSucActual);

    // Barra de progreso
    QProgressBar *barraProg = new QProgressBar();
    barraProg->setRange(0, 100);
    barraProg->setValue(0);
    barraProg->setStyleSheet(QString(
                                 "QProgressBar { border: 1px solid %1; border-radius: 4px;"
                                 " text-align: center; color: %2; background: white;}"
                                 "QProgressBar::chunk { background: %3; }"
                                 ).arg(GRIS_BORDE).arg(TEXTO_NEGRO).arg(AZUL_MEDIO));
    estLay->addWidget(barraProg);

    // Log de eventos
    QLabel *lblLogTit = new QLabel("Log de eventos:");
    lblLogTit->setStyleSheet(QString(
                                 "color: %1; font-weight: bold; font-size: 11px; padding-top: 6px;"
                                 ).arg(TEXTO_NEGRO));
    estLay->addWidget(lblLogTit);

    QTextEdit *txtLog = new QTextEdit();
    txtLog->setReadOnly(true);
    txtLog->setStyleSheet(estiloTextEdit());
    txtLog->setFixedHeight(110);
    estLay->addWidget(txtLog);

    pLay->addWidget(gbEstado);

    // Panel resumen de colas (números actuales por sucursal)
    QGroupBox *gbColas = new QGroupBox("Colas por sucursal");
    gbColas->setStyleSheet(estiloGroupBox());
    QVBoxLayout *colasLay = new QVBoxLayout(gbColas);
    colasLay->setContentsMargins(12, 16, 12, 12);
    QTextEdit *txtColas = new QTextEdit();
    txtColas->setReadOnly(true);
    txtColas->setStyleSheet(estiloTextEdit());
    txtColas->setFixedHeight(85);
    colasLay->addWidget(txtColas);
    pLay->addWidget(gbColas);
    pLay->addStretch();

    scrollPanel->setWidget(panel);
    lay->addWidget(scrollPanel);

    // Panel derecho
    QWidget *panelRes = new QWidget();
    QVBoxLayout *rLay = new QVBoxLayout(panelRes);
    rLay->setSpacing(12);

    QGroupBox *gbRuta = new QGroupBox("Ruta óptima");
    gbRuta->setStyleSheet(estiloGroupBox());
    QVBoxLayout *rutaLay = new QVBoxLayout(gbRuta);
    rutaLay->setContentsMargins(12, 16, 12, 12);

    QGraphicsScene *sceneRuta = new QGraphicsScene();
    sceneRuta->setBackgroundBrush(Qt::white);
    QGraphicsView *viewRuta = new QGraphicsView(sceneRuta);
    viewRuta->setRenderHint(QPainter::Antialiasing);
    viewRuta->setFixedHeight(220);
    viewRuta->setStyleSheet(QString("border: 1px solid %1; background: white;").arg(GRIS_BORDE));
    rutaLay->addWidget(viewRuta);
    rLay->addWidget(gbRuta);

    QGroupBox *gbDet = new QGroupBox("Detalles");
    gbDet->setStyleSheet(estiloGroupBox());
    QVBoxLayout *detLay = new QVBoxLayout(gbDet);
    detLay->setContentsMargins(12, 16, 12, 12);
    QTextEdit *txtDet = new QTextEdit();
    txtDet->setReadOnly(true);
    txtDet->setStyleSheet(estiloTextEdit());
    detLay->addWidget(txtDet);
    rLay->addWidget(gbDet, 1);
    lay->addWidget(panelRes, 1);

    struct EstadoRuta {
        ResultadoRuta ruta;
        QString codigoBarra, origenId, destinoId;
    };
    EstadoRuta *estado = new EstadoRuta();

    auto refrescarCombosYColas = [=]() {
        QString prevO = cOrigen->currentData().toString();
        QString prevD = cDestino->currentData().toString();
        cOrigen->clear(); cDestino->clear();
        for (Sucursal *s : red->obtenerSucursales()) {
            QString lbl = QString::fromStdString(s->getId() + " — " + s->getNombre());
            QString id  = QString::fromStdString(s->getId());
            cOrigen->addItem(lbl, id);
            cDestino->addItem(lbl, id);
        }
        for (int i = 0; i < cOrigen->count(); i++)
            if (cOrigen->itemData(i).toString() == prevO) {
                cOrigen->setCurrentIndex(i); break;
            }
        for (int i = 0; i < cDestino->count(); i++)
            if (cDestino->itemData(i).toString() == prevD) {
                cDestino->setCurrentIndex(i); break;
            }
        if (cDestino->count() > 1 && cDestino->currentIndex() == 0)
            cDestino->setCurrentIndex(1);

        // Estado de colas
        QString info;
        for (Sucursal *s : red->obtenerSucursales()) {
            info += QString("[%1]\n  Ingreso:  %2\n  Traspaso: %3\n  Salida:   %4\n\n")
            .arg(QString::fromStdString(s->getId()))
                .arg(s->getColaIngreso().obtenerTamano())
                .arg(s->getColaTraspaso().obtenerTamano())
                .arg(s->getColaSalida().obtenerTamano());
        }
        if (info.isEmpty()) info = "Sin sucursales cargadas.";
        txtColas->setText(info);
    };

    connect(btnCalc, &QPushButton::clicked, [=]() {
        QString o = cOrigen->currentData().toString();
        QString d = cDestino->currentData().toString();
        QString cod = eCodigo->text().trimmed();

        if (o == d) { QMessageBox::warning(w, "Error",
                                 "Origen y destino deben ser distintos."); return; }

        RedSucursales::Criterio crit = rTiempo->isChecked()
                                           ? RedSucursales::TIEMPO : RedSucursales::COSTO;
        ResultadoRuta r = red->rutaOptima(o.toStdString(), d.toStdString(), crit);
        estado->ruta = r; estado->codigoBarra = cod;
        estado->origenId = o; estado->destinoId = d;

        sceneRuta->clear();
        if (!r.encontrada) {
            QGraphicsTextItem *t = sceneRuta->addText(
                "No existe ruta entre las sucursales.");
            t->setDefaultTextColor(Qt::red);
            txtDet->setText("Sin ruta encontrada.");
            btnTrans->setEnabled(false);
            return;
        }

        int n = (int)r.nodos.size();
        const double esp = 140.0, cy = 80.0;
        double x0 = 60.0;
        for (int i = 0; i < n; i++) {
            double x = x0 + i * esp;
            Sucursal *s = red->buscarSucursal(r.nodos[i]);
            QColor color = (i == 0 || i == n-1) ? QColor("#FFD600") : QColor(AZUL_CLARO);
            sceneRuta->addEllipse(x - 30, cy - 30, 60, 60,
                                  QPen(QColor(AZUL_OSCURO), 2), QBrush(color));
            QGraphicsTextItem *ti = sceneRuta->addText(QString::fromStdString(r.nodos[i]));
            ti->setFont(QFont("Arial", 9, QFont::Bold));
            ti->setDefaultTextColor(QColor(AZUL_OSCURO));
            ti->setPos(x - ti->boundingRect().width()/2, cy - 10);
            if (s) {
                QGraphicsTextItem *tn = sceneRuta->addText(
                    QString::fromStdString(s->getNombre()));
                tn->setFont(QFont("Arial", 8));
                tn->setDefaultTextColor(QColor("#37474F"));
                tn->setPos(x - 50, cy + 36);
            }
            if (i < n - 1) {
                sceneRuta->addLine(x + 30, cy, x0 + (i+1)*esp - 30, cy,
                                   QPen(QColor("#E53935"), 2.5));
                for (const Conexion &c : red->obtenerConexionesDe(r.nodos[i])) {
                    if (c.destinoId == r.nodos[i+1]) {
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
        viewRuta->fitInView(sceneRuta->sceneRect().adjusted(-20, -20, 20, 20),
                            Qt::KeepAspectRatio);

        double eta = red->calcularETA(r, crit);
        QString det;
        det += QString("=== Ruta óptima (%1) ===\n")
                   .arg(crit == RedSucursales::TIEMPO ? "mínimo tiempo" : "mínimo costo");
        det += "Camino: ";
        for (const auto &nid : r.nodos) det += QString::fromStdString(nid) + " → ";
        det.chop(3);
        det += QString("\n\nPeso total : %1 %2\n")
                   .arg(r.pesoTotal)
                   .arg(crit == RedSucursales::TIEMPO ? "segundos" : "Q");
        det += QString("ETA total  : %1 segundos\n").arg(eta);
        det += QString("Nodos      : %1\n\nDetalle por tramo:\n").arg(n);
        for (int i = 0; i < n - 1; i++) {
            for (const Conexion &c : red->obtenerConexionesDe(r.nodos[i])) {
                if (c.destinoId == r.nodos[i+1]) {
                    det += QString("  %1 → %2 : t=%3s  Q%4\n")
                               .arg(QString::fromStdString(c.origenId))
                               .arg(QString::fromStdString(c.destinoId))
                               .arg(c.tiempo).arg(c.costo);
                    break;
                }
            }
        }
        if (!cod.isEmpty()) {
            Sucursal *sO = red->buscarSucursal(o.toStdString());
            Producto *p = sO ? sO->buscarPorCodigo(cod.toStdString()) : nullptr;
            if (p) {
                det += QString("\nProducto: %1 (%2)\nEstado: %3\nStock disponible: %4 unidades\n")
                .arg(QString::fromStdString(p->nombre)).arg(cod)
                    .arg(QString::fromStdString(p->estado))
                    .arg(p->stock);
                eUnidades->setMaximum(p->stock);
                eUnidades->setValue(p->stock > 1 ? 1 : p->stock);
                btnTrans->setEnabled(true);
            } else {
                det += "\n[!] Producto no encontrado en el origen.\n";
                btnTrans->setEnabled(false);
            }
        } else {
            det += "\n[!] Ingresa un código para habilitar la transferencia.\n";
            btnTrans->setEnabled(false);
        }
        txtDet->setText(det);
    });

    // Simulador persistente (vive lo mismo que el tab)
    SimuladorTransferencia *sim = new SimuladorTransferencia(w);

    // Función para refrescar el panel "Colas por sucursal" en vivo
    auto refrescarPanelColas = [=]() {
        QString info;
        for (Sucursal *s : red->obtenerSucursales()) {
            int ing = s->getColaIngreso().obtenerTamano();
            int tra = s->getColaTraspaso().obtenerTamano();
            int sal = s->getColaSalida().obtenerTamano();
            // Resaltar sucursales con productos en sus colas
            QString prefix = (ing+tra+sal > 0) ? "►" : " ";
            info += QString("%1 [%2]  ing:%3  tras:%4  sal:%5\n")
                        .arg(prefix).arg(QString::fromStdString(s->getId()))
                        .arg(ing).arg(tra).arg(sal);
        }
        if (info.isEmpty()) info = "Sin sucursales.";
        txtColas->setText(info);
    };

    // Color del badge según etapa
    auto colorEtapa = [](int etapa) -> QString {
        switch (etapa) {
        case SimuladorTransferencia::INGRESO:   return "#1976D2"; // azul
        case SimuladorTransferencia::TRASPASO:  return "#F57C00"; // naranja
        case SimuladorTransferencia::DESPACHO:  return "#7B1FA2"; // morado
        case SimuladorTransferencia::EN_VIAJE:  return "#C62828"; // rojo
        case SimuladorTransferencia::ENTREGADO: return "#2E7D32"; // verde
        default: return "#888";
        }
    };
    auto textoEtapa = [](int etapa) -> QString {
        switch (etapa) {
        case SimuladorTransferencia::INGRESO:   return "EN INGRESO";
        case SimuladorTransferencia::TRASPASO:  return "EN TRASPASO";
        case SimuladorTransferencia::DESPACHO:  return "EN DESPACHO";
        case SimuladorTransferencia::EN_VIAJE:  return "EN VIAJE";
        case SimuladorTransferencia::ENTREGADO: return "ENTREGADO";
        default: return "—";
        }
    };

    // Conectar las señales del simulador con la GUI
    QObject::connect(sim, &SimuladorTransferencia::log,
                     [=](const QString &msg) {
                         QString hora = QDateTime::currentDateTime().toString("HH:mm:ss");
                         txtLog->append(QString("[%1] %2").arg(hora).arg(msg));
                         refrescarPanelColas();
                     });

    QObject::connect(sim, &SimuladorTransferencia::etapaIniciada,
                     [=](const QString &sucId, int etapa, const QString &mensaje) {
                         lblSucActual->setText("Sucursal actual: " + sucId);
                         lblEstadoProd->setText(textoEtapa(etapa));
                         lblEstadoProd->setStyleSheet(QString(
                                                          "background: %1; color: white; font-weight: bold;"
                                                          " padding: 8px; border-radius: 4px; font-size: 13px;"
                                                          ).arg(colorEtapa(etapa)));

                         // Calcular progreso aproximado
                         int n = (int)estado->ruta.nodos.size();
                         int idxNodo = estado->ruta.nodos.size();
                         for (int i = 0; i < n; i++) {
                             if (QString::fromStdString(estado->ruta.nodos[i]) == sucId) {
                                 idxNodo = i; break;
                             }
                         }
                         int prog = (n > 1) ? (idxNodo * 100) / (n - 1) : 100;
                         barraProg->setValue(prog);
                         barraProg->setFormat(QString("Tramo %1 de %2 — %p%")
                                                  .arg(idxNodo + 1).arg(n));
                         (void)mensaje;
                     });

    QObject::connect(sim, &SimuladorTransferencia::productoEnTransito,
                     [=](const QString &origen, const QString &destino) {
                         lblSucActual->setText(
                             QString("En tránsito: %1 → %2").arg(origen).arg(destino));
                     });

    QObject::connect(sim, &SimuladorTransferencia::simulacionCompletada,
                     [=](bool exitosa) {
                         btnTrans->setEnabled(false);
                         btnCancel->setEnabled(false);
                         btnCalc->setEnabled(true);
                         cOrigen->setEnabled(true);
                         cDestino->setEnabled(true);
                         eCodigo->setEnabled(true);
                         eUnidades->setEnabled(true);

                         if (exitosa) {
                             lblEstadoProd->setText("ENTREGADO ✓");
                             lblEstadoProd->setStyleSheet(QString(
                                                              "background: %1; color: white; font-weight: bold;"
                                                              " padding: 8px; border-radius: 4px; font-size: 13px;"
                                                              ).arg("#2E7D32"));
                             barraProg->setValue(100);
                             barraProg->setFormat("Completada");
                             emit redActualizada();
                         } else {
                             lblEstadoProd->setText("CANCELADA");
                             lblEstadoProd->setStyleSheet(QString(
                                                              "background: %1; color: white; font-weight: bold;"
                                                              " padding: 8px; border-radius: 4px; font-size: 13px;"
                                                              ).arg("#888"));
                             barraProg->setValue(0);
                         }
                         refrescarPanelColas();
                     });

    // Botón Simular: arranca la simulación animada
    connect(btnTrans, &QPushButton::clicked, [=]() {
        if (!estado->ruta.encontrada) return;
        RedSucursales::Criterio crit = rTiempo->isChecked()
                                           ? RedSucursales::TIEMPO : RedSucursales::COSTO;

        // Limpiar log y resetear UI
        txtLog->clear();
        barraProg->setValue(0);
        lblEstadoProd->setText("INICIANDO...");
        lblEstadoProd->setStyleSheet(QString(
                                         "background: %1; color: white; font-weight: bold;"
                                         " padding: 8px; border-radius: 4px; font-size: 13px;"
                                         ).arg(AZUL_MEDIO));

        // Bloquear inputs durante la simulación
        btnTrans->setEnabled(false);
        btnCalc->setEnabled(false);
        btnCancel->setEnabled(true);
        cOrigen->setEnabled(false);
        cDestino->setEnabled(false);
        eCodigo->setEnabled(false);
        eUnidades->setEnabled(false);

        sim->iniciar(red,
                     estado->codigoBarra.toStdString(),
                     estado->origenId.toStdString(),
                     estado->destinoId.toStdString(),
                     crit,
                     eUnidades->value(),
                     cmbVelocidad->currentData().toInt());
    });

    // Botón Cancelar
    connect(btnCancel, &QPushButton::clicked, [=]() { sim->cancelar(); });

    // Inicializar panel de colas
    refrescarPanelColas();

    connect(this, &MainWindow::redActualizada, refrescarCombosYColas);
    refrescarCombosYColas();
    return w;
}

// ════════════════════════════════════════════════════════════
// TAB 6 — RENDIMIENTO
// ════════════════════════════════════════════════════════════
QWidget* MainWindow::crearTabRendimiento() {
    QWidget *w = new QWidget();
    w->setStyleSheet("background: white;");
    QVBoxLayout *lay = new QVBoxLayout(w);
    lay->setContentsMargins(20, 20, 20, 20);
    lay->setSpacing(14);

    QString cstyle = estiloCampo();

    QLabel *titulo = new QLabel("Medición de Rendimiento");
    titulo->setStyleSheet(QString(
                              "color: %1; font-size: 16px; font-weight: bold;"
                              " border-bottom: 2px solid %2; padding-bottom: 8px;"
                              ).arg(AZUL_OSCURO).arg(AZUL_MEDIO));
    lay->addWidget(titulo);

    QHBoxLayout *confLay = new QHBoxLayout();
    QLabel *lblSuc = new QLabel("Sucursal:");
    lblSuc->setStyleSheet(QString("color: %1; font-weight: bold;").arg(TEXTO_NEGRO));
    QComboBox *cmbSuc = new QComboBox();
    cmbSuc->setStyleSheet(cstyle);
    QPushButton *btnEjecutar = new QPushButton("▶  Ejecutar benchmark");
    btnEjecutar->setStyleSheet(estiloBoton(AZUL_MEDIO));
    btnEjecutar->setFixedWidth(220);
    confLay->addWidget(lblSuc);
    confLay->addWidget(cmbSuc, 1);
    confLay->addWidget(btnEjecutar);
    lay->addLayout(confLay);

    QTableWidget *tbl = new QTableWidget(4, 5);
    tbl->setHorizontalHeaderLabels(
        {"Caso", "Lista Simple (µs)", "Lista Ordenada (µs)",
         "Árbol AVL (µs)", "Hash (µs)"});
    tbl->verticalHeader()->setVisible(false);
    tbl->horizontalHeader()->setStretchLastSection(true);
    tbl->horizontalHeader()->setStyleSheet(estiloHeader());
    tbl->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tbl->setAlternatingRowColors(true);
    tbl->setStyleSheet(estiloTabla());
    QStringList casos = {
                         "Búsqueda exitosa aleatoria", "Búsqueda fallida",
                         "Extremos (primero/último)", "Peor caso (último A-Z)"};
    for (int i = 0; i < 4; i++) {
        auto mk = [](const QString &v) {
            QTableWidgetItem *it = new QTableWidgetItem(v);
            it->setForeground(QColor(TEXTO_NEGRO));
            return it;
        };
        tbl->setItem(i, 0, mk(casos[i]));
        for (int j = 1; j < 5; j++) tbl->setItem(i, j, mk("—"));
    }
    tbl->resizeColumnsToContents();
    lay->addWidget(tbl);

    QGroupBox *gbA = new QGroupBox("Análisis de factores de aceleración");
    gbA->setStyleSheet(estiloGroupBox());
    QVBoxLayout *aLay = new QVBoxLayout(gbA);
    aLay->setContentsMargins(12, 16, 12, 12);
    QTextEdit *txtA = new QTextEdit();
    txtA->setReadOnly(true);
    txtA->setStyleSheet(estiloTextEdit());
    txtA->setFixedHeight(140);
    aLay->addWidget(txtA);
    lay->addWidget(gbA);
    lay->addStretch();

    auto refrescarCombo = [=]() {
        cmbSuc->clear();
        for (Sucursal *s : red->obtenerSucursales())
            cmbSuc->addItem(QString::fromStdString(s->getId() + " — " + s->getNombre()),
                            QString::fromStdString(s->getId()));
    };

    connect(btnEjecutar, &QPushButton::clicked, [=]() {
        if (cmbSuc->count() == 0) {
            QMessageBox::warning(w, "Atención", "No hay sucursales cargadas.");
            return;
        }
        QString sucId = cmbSuc->currentData().toString();
        Sucursal *s = red->buscarSucursal(sucId.toStdString());
        if (!s || s->contarProductos() < 20) {
            QMessageBox::warning(w, "Atención",
                                 QString("Se necesitan al menos 20 productos en la sucursal (hay %1).")
                                     .arg(s ? s->contarProductos() : 0));
            return;
        }

        std::vector<std::string> nombres, codigos;
        struct W {
            static void walk(NodoAVL *n, std::vector<std::string> &v,
                             std::vector<std::string> &c) {
                if (!n || v.size() >= 20) return;
                walk(n->izquierda, v, c);
                if (v.size() < 20) {
                    v.push_back(n->dato.nombre);
                    c.push_back(n->dato.codigoBarra);
                }
                walk(n->derecha, v, c);
            }
        };
        W::walk(s->getCatalogo()->obtenerArbolAVL()->obtenerRaiz(), nombres, codigos);
        if (nombres.size() < 20) return;

        const int N = 20, M = 5;
        using Clock = std::chrono::high_resolution_clock;
        auto medir = [&](auto fn, const std::vector<std::string> &xs) {
            double total = 0;
            for (int rep = 0; rep < M; rep++) {
                auto t0 = Clock::now();
                for (int i = 0; i < N; i++) fn(xs[i % xs.size()]);
                total += std::chrono::duration<double, std::micro>(
                             Clock::now() - t0).count();
            }
            return total / M / N;
        };

        std::vector<std::string> fal = {"Zzzzz No Existe", "XXXXX Fantasma",
                                        "Producto Inexistente", "AAA Falso", "ZZZ Nada"};
        std::vector<std::string> falCods = {"0000000001", "9999999999",
                                            "1234567890", "0987654321", "5555555555"};
        std::vector<std::string> ext = {nombres.front(), nombres.back()};
        std::vector<std::string> extC = {codigos.front(), codigos.back()};
        std::vector<std::string> peor = {nombres.back()};
        std::vector<std::string> peorC = {codigos.back()};

        Catalogo *cat = s->getCatalogo();
        auto fLS  = [&](const std::string &x) { cat->obtenerListaSimple()->buscarPorNombre(x); };
        auto fLO  = [&](const std::string &x) { cat->obtenerListaOrdenada()->buscarPorNombre(x); };
        auto fAVL = [&](const std::string &x) { cat->obtenerArbolAVL()->buscar(x); };
        auto fH   = [&](const std::string &x) { cat->obtenerTablaHash()->buscar(x); };

        struct { double ls, lo, avl, h; } R[4];
        R[0] = {medir(fLS, nombres), medir(fLO, nombres),
                medir(fAVL, nombres), medir(fH, codigos)};
        R[1] = {medir(fLS, fal), medir(fLO, fal),
                medir(fAVL, fal), medir(fH, falCods)};
        R[2] = {medir(fLS, ext), medir(fLO, ext),
                medir(fAVL, ext), medir(fH, extC)};
        R[3] = {medir(fLS, peor), medir(fLO, peor),
                medir(fAVL, peor), medir(fH, peorC)};

        for (int i = 0; i < 4; i++) {
            tbl->item(i, 1)->setText(QString::number(R[i].ls,  'f', 3));
            tbl->item(i, 2)->setText(QString::number(R[i].lo,  'f', 3));
            tbl->item(i, 3)->setText(QString::number(R[i].avl, 'f', 3));
            tbl->item(i, 4)->setText(QString::number(R[i].h,   'f', 3));
        }
        tbl->resizeColumnsToContents();

        double avl0 = R[0].avl > 0 ? R[0].avl : 0.001;
        double h0   = R[0].h > 0 ? R[0].h : 0.001;
        QString t;
        t += "=== Factores de aceleración (búsqueda exitosa) ===\n";
        t += QString("Lista Simple   vs AVL  : %1x mas lenta\n").arg(R[0].ls / avl0, 0, 'f', 1);
        t += QString("Lista Ordenada vs AVL  : %1x mas lenta\n").arg(R[0].lo / avl0, 0, 'f', 1);
        t += QString("Lista Simple   vs Hash : %1x mas lenta\n").arg(R[0].ls / h0, 0, 'f', 1);
        t += QString("\nn=%1 -> log2(n) = %2 niveles en AVL | Hash O(1) amortizado\n")
                 .arg(s->contarProductos()).arg(std::log2(s->contarProductos()), 0, 'f', 1);
        txtA->setText(t);
    });

    connect(this, &MainWindow::redActualizada, refrescarCombo);
    refrescarCombo();
    return w;
}

// ════════════════════════════════════════════════════════════
// TAB 7 — VISUALIZACIÓN
// ════════════════════════════════════════════════════════════
QWidget* MainWindow::crearTabEstructuras() {
    QWidget *w = new QWidget();
    w->setStyleSheet("background: white;");
    QVBoxLayout *lay = new QVBoxLayout(w);
    lay->setContentsMargins(12, 12, 12, 12);
    lay->setSpacing(10);

    QString cstyle = estiloCampo();

    QHBoxLayout *ctrlLay = new QHBoxLayout();
    QLabel *lblSuc = new QLabel("Sucursal:");
    lblSuc->setStyleSheet(QString("color: %1; font-weight: bold;").arg(TEXTO_NEGRO));
    QComboBox *cmbSuc = new QComboBox();
    cmbSuc->setStyleSheet(cstyle);
    QLabel *lblArb = new QLabel("Árbol:");
    lblArb->setStyleSheet(QString("color: %1; font-weight: bold;").arg(TEXTO_NEGRO));
    QComboBox *cmbArbol = new QComboBox();
    cmbArbol->addItems({"Árbol AVL", "Árbol B", "Árbol B+", "Grafo de red"});
    cmbArbol->setStyleSheet(cstyle);
    cmbArbol->setFixedWidth(160);

    QPushButton *btnGen  = new QPushButton("⟳  Generar .dot");
    QPushButton *btnConv = new QPushButton("🖼  Convertir a PNG");
    btnGen->setStyleSheet(estiloBoton(AZUL_MEDIO));
    btnConv->setStyleSheet(estiloBoton(NARANJA));

    ctrlLay->addWidget(lblSuc);
    ctrlLay->addWidget(cmbSuc, 1);
    ctrlLay->addWidget(lblArb);
    ctrlLay->addWidget(cmbArbol);
    ctrlLay->addWidget(btnGen);
    ctrlLay->addWidget(btnConv);
    lay->addLayout(ctrlLay);

    QLabel *lblImg = new QLabel("Genera el archivo .dot y conviértelo a PNG para visualizar.");
    lblImg->setAlignment(Qt::AlignCenter);
    lblImg->setStyleSheet(QString(
                              "color: %1; font-size: 13px; background: %2;"
                              " border: 1px solid %3; border-radius: 4px; padding: 30px;"
                              ).arg("#888").arg(GRIS_CLARO).arg(GRIS_BORDE));
    lblImg->setWordWrap(true);
    lay->addWidget(lblImg, 1);

    QLabel *lblRuta = new QLabel();
    lblRuta->setStyleSheet(QString("color: %1; font-size: 11px;").arg(TEXTO_NEGRO));
    lay->addWidget(lblRuta);

    auto refrescarCombo = [=]() {
        cmbSuc->clear();
        for (Sucursal *s : red->obtenerSucursales())
            cmbSuc->addItem(QString::fromStdString(s->getId() + " — " + s->getNombre()),
                            QString::fromStdString(s->getId()));
    };

    connect(btnGen, &QPushButton::clicked, [=]() {
        if (red->contarSucursales() == 0 && cmbArbol->currentIndex() != 3) {
            QMessageBox::warning(w, "Atención", "No hay sucursales cargadas."); return;
        }
        QString sucId = cmbSuc->currentData().toString();
        int arb = cmbArbol->currentIndex();
        QString rutaDot;
        if (arb == 3) {
            rutaDot = "output/grafo_red.dot";
            red->generarDot(rutaDot.toStdString());
        } else {
            Sucursal *s = red->buscarSucursal(sucId.toStdString());
            if (!s) return;
            VisualizadorDot viz("output");
            Catalogo *cat = s->getCatalogo();
            if (arb == 0) { rutaDot = "output/avl.dot";    viz.generarAVL(cat->obtenerArbolAVL(), 30); }
            else if (arb == 1) { rutaDot = "output/arbolB.dot";  viz.generarArbolB(cat->obtenerArbolB(), 20); }
            else { rutaDot = "output/arbolBP.dot"; viz.generarArbolBPlus(cat->obtenerArbolBPlus(), 20); }
        }
        lblRuta->setText("Archivo generado: " + rutaDot);
        lblImg->setText(
            QString("Archivo .dot generado en:\n%1\n\n"
                    "Para convertir a PNG, presiona 'Convertir a PNG'.\n"
                    "(requiere Graphviz instalado)").arg(rutaDot));
    });

    connect(btnConv, &QPushButton::clicked, [=]() {
        QString r = lblRuta->text().replace("Archivo generado: ", "");
        if (r.isEmpty()) {
            QMessageBox::information(w, "Info", "Primero genera el .dot."); return;
        }
        QString png = r; png.replace(".dot", ".png");
        QProcess proc;
        proc.start("dot", {"-Tpng", r, "-o", png});
        proc.waitForFinished(8000);
        if (proc.exitCode() == 0) {
            QPixmap px(png);
            if (!px.isNull()) {
                lblImg->setPixmap(px.scaled(lblImg->size(),
                                            Qt::KeepAspectRatio, Qt::SmoothTransformation));
                lblImg->setText("");
            }
        } else {
            QMessageBox::warning(w, "Error",
                                 "No se pudo convertir. Instala Graphviz desde:\n"
                                 "https://graphviz.org/download/");
        }
    });

    connect(this, &MainWindow::redActualizada, refrescarCombo);
    refrescarCombo();
    return w;
}
