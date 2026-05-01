#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTabWidget>
#include <QLabel>
#include <QStatusBar>
#include "RedSucursales.h"
#include "CargadorRed.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

/*
 * MainWindow — orquestador de la GUI
 * ------------------------------------
 * Posee la RedSucursales (fuente única de datos).
 * Carga los CSV al arrancar.
 * Cada tab recibe un puntero a la red para leer y modificar.
 * Las señales actualizarRed() propagan cambios entre tabs.
 */
class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

signals:
    // Se emite cuando cualquier tab modifica la red
    void redActualizada();

private slots:
    void onRedActualizada();
    void onTabCambiado(int index);

private:
    Ui::MainWindow *ui;
    RedSucursales  *red;

    QTabWidget *tabs;
    QLabel     *lblEstado;  // barra de estado inferior

    void cargarDatos();
    void construirUI();
    void actualizarBarraEstado();

    // Builders de cada tab
    QWidget* crearTabSistema();
    QWidget* crearTabSucursales();
    QWidget* crearTabRed();
    QWidget* crearTabInventario();
    QWidget* crearTabTransferencia();
    QWidget* crearTabRendimiento();
    QWidget* crearTabEstructuras();

    // Helpers de estilo
    static QString estiloBoton(const QString &color);
    static QString estiloHeader();
};

#endif // MAINWINDOW_H
