#include "mainwindow.h"
#include <QApplication>

/*
 * main.cpp — Fase 2
 * ------------------
 * Punto de entrada de la aplicación Qt.
 *
 * QApplication maneja el event loop y los recursos gráficos.
 * MainWindow es la ventana principal que contiene toda la UI.
 *
 * La lógica del catálogo, sucursales y estructuras de datos
 * vive en core/ y se instancia desde MainWindow.
 */

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    app.setApplicationName("Gestión de Catálogo — Red de Sucursales");
    app.setApplicationVersion("2.0");
    app.setOrganizationName("Curso Estructura de Datos");

    MainWindow ventana;
    ventana.setWindowTitle("Gestión de Catálogo — Red de Sucursales");
    ventana.resize(1200, 800);
    ventana.show();

    return app.exec();
}
