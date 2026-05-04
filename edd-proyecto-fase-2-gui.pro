# ============================================================
# edd-proyecto-fase-2-gui.pro
# Gestión de Catálogo — Red de Sucursales  |  Fase 2
# ============================================================

QT += widgets

CONFIG += c++17
CONFIG -= app_bundle

TARGET   = edd-proyecto-fase-2-gui
TEMPLATE = app

# ============================================================
# Include paths
# ============================================================
INCLUDEPATH += \
    $$PWD/core/include \
    $$PWD/include

# ============================================================
# Headers — core/ (Fase 1)
# ============================================================
HEADERS += \
    $$PWD/core/include/Producto.h           \
    $$PWD/core/include/NodoLista.h          \
    $$PWD/core/include/ListaSimple.h        \
    $$PWD/core/include/ListaOrdenada.h      \
    $$PWD/core/include/NodoAVL.h            \
    $$PWD/core/include/ArbolAVL.h           \
    $$PWD/core/include/NodoB.h              \
    $$PWD/core/include/ArbolB.h             \
    $$PWD/core/include/NodoBPlus.h          \
    $$PWD/core/include/ArbolBPlus.h         \
    $$PWD/core/include/NodoHash.h           \
    $$PWD/core/include/TablaHash.h          \
    $$PWD/core/include/Catalogo.h           \
    $$PWD/core/include/CargadorCSV.h        \
    $$PWD/core/include/MedidorRendimiento.h \
    $$PWD/core/include/VisualizadorDot.h

# ============================================================
# Headers — include/ (Fase 2)
# ============================================================
HEADERS += \
    mainwindow.h                      \
    $$PWD/include/Cola.h              \
    $$PWD/include/Pila.h              \
    $$PWD/include/Operacion.h         \
    $$PWD/include/Sucursal.h          \
    $$PWD/include/RedSucursales.h

# ============================================================
# Sources — core/ (Fase 1)
# ============================================================
SOURCES += \
    $$PWD/core/src/Producto.cpp           \
    $$PWD/core/src/NodoLista.cpp          \
    $$PWD/core/src/ListaSimple.cpp        \
    $$PWD/core/src/ListaOrdenada.cpp      \
    $$PWD/core/src/NodoAVL.cpp            \
    $$PWD/core/src/ArbolAVL.cpp           \
    $$PWD/core/src/NodoB.cpp              \
    $$PWD/core/src/ArbolB.cpp             \
    $$PWD/core/src/NodoBPlus.cpp          \
    $$PWD/core/src/ArbolBPlus.cpp         \
    $$PWD/core/src/NodoHash.cpp           \
    $$PWD/core/src/TablaHash.cpp          \
    $$PWD/core/src/Catalogo.cpp           \
    $$PWD/core/src/CargadorCSV.cpp        \
    $$PWD/core/src/MedidorRendimiento.cpp \
    $$PWD/core/src/VisualizadorDot.cpp

# ============================================================
# Sources — src/ (Fase 2)
# ============================================================
SOURCES += \
    main.cpp                          \
    mainwindow.cpp                    \
    $$PWD/src/Sucursal.cpp            \
    $$PWD/src/RedSucursales.cpp

# ============================================================
# Forms
# ============================================================
FORMS += mainwindow.ui

# ============================================================
# Instalación
# ============================================================
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

# Fase 2 — CargadorRed
HEADERS += $$PWD/include/CargadorRed.h
SOURCES += $$PWD/src/CargadorRed.cpp


# Fase 4 — Simulador de transferencia con animación
HEADERS += $$PWD/include/SimuladorTransferencia.h
SOURCES += $$PWD/src/SimuladorTransferencia.cpp
