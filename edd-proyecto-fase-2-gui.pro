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
# Include paths — orden importante:
#   1. core/include  → para que los #include entre clases core funcionen
#   2. include       → clases nuevas de Fase 2
# ============================================================
INCLUDEPATH += \
    $$PWD/core/include \
    $$PWD/include

# ============================================================
# Headers
# ============================================================
HEADERS += \
    mainwindow.h \
    \
    $$PWD/core/include/Producto.h          \
    $$PWD/core/include/NodoLista.h         \
    $$PWD/core/include/ListaSimple.h       \
    $$PWD/core/include/ListaOrdenada.h     \
    $$PWD/core/include/NodoAVL.h           \
    $$PWD/core/include/ArbolAVL.h          \
    $$PWD/core/include/NodoB.h             \
    $$PWD/core/include/ArbolB.h            \
    $$PWD/core/include/NodoBPlus.h         \
    $$PWD/core/include/ArbolBPlus.h        \
    $$PWD/core/include/NodoHash.h          \
    $$PWD/core/include/TablaHash.h         \
    $$PWD/core/include/Catalogo.h          \
    $$PWD/core/include/CargadorCSV.h       \
    $$PWD/core/include/MedidorRendimiento.h \
    $$PWD/core/include/VisualizadorDot.h

# ============================================================
# Sources
# ============================================================
SOURCES += \
    main.cpp         \
    mainwindow.cpp   \
    \
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
# Forms
# ============================================================
FORMS += mainwindow.ui

# ============================================================
# Instalación
# ============================================================
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
