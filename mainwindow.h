#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QVBoxLayout>
#include <QTabWidget>

// Incluimos el núcleo de datos para verificar el enlace en compilación
#include "Catalogo.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

/*
 * MainWindow.h
 * -------------
 * Ventana principal de la aplicación.
 *
 * Por ahora muestra una pantalla de bienvenida con las
 * estadísticas del catálogo de prueba para confirmar que
 * core/ está correctamente enlazado con Qt.
 *
 * En las fases siguientes cada pestaña del QTabWidget
 * se reemplazará por los módulos reales:
 *   - Sucursales
 *   - Red (grafo)
 *   - Inventario
 *   - Transferencia
 *   - Rendimiento
 *   - Visualización
 */

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private:
    Ui::MainWindow *ui;

    // Catálogo de prueba — confirma que core/ enlaza correctamente
    Catalogo *catalogoPrueba;

    // Construye la pantalla de bienvenida
    void inicializarUI();
    void construirPantallaBienvenida(QWidget *contenedor);
};

#endif // MAINWINDOW_H
