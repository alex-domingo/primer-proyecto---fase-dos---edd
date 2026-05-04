#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTabWidget>
#include <QLabel>
#include <QStatusBar>
#include "RedSucursales.h"
#include "CargadorRed.h"
#include "VisualizadorDot.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

/*
 * MainWindow — orquestador de la GUI
 * ------------------------------------
 * El sistema arranca con la red VACÍA. El usuario debe usar los
 * botones del Tab Sistema para cargar los CSV.
 *
 * Cuando algo cambia en la red, se emite redActualizada(). Cada
 * tab se conecta a esa señal y reconstruye sus widgets dinámicos
 * (combos de sucursal, listas, tablas) desde cero con los datos
 * actuales — eso garantiza que las sucursales nuevas aparezcan
 * en todos lados.
 */
class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

signals:
    void redActualizada();

private slots:
    void onRedActualizada();

private:
    Ui::MainWindow *ui;
    RedSucursales  *red;

    QTabWidget *tabs;
    QLabel     *lblEstado;
    QLabel     *lblHeaderStats;

    void construirUI();
    void actualizarBarraEstado();

    QWidget* crearTabSistema();
    QWidget* crearTabSucursales();
    QWidget* crearTabRed();
    QWidget* crearTabInventario();
    QWidget* crearTabTransferencia();
    QWidget* crearTabRendimiento();
    QWidget* crearTabEstructuras();

    static QString estiloBoton(const QString &color);
    static QString estiloHeader();
    static QString estiloTabla();
    static QString estiloCampo();
};

#endif // MAINWINDOW_H
