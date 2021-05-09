#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QTableWidget>
#include <QTextBrowser>
#include <QThread>

#include "Fungera.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

    QWidget *central_window;
    QPushButton *toggle_btn, *cycle_btn,*next_btn,*prev_btn;
    QTableWidget *simulation_stats;
    QTableWidget *memory_view;
    QThread *simulation_thread;
    Fungera *simulation;
    size_t selected_organism;

public:
    MainWindow(Fungera *simulation,QWidget *parent = nullptr);
    ~MainWindow();

    void setup_gui();

protected:
    const Organism &get_selected_organism();
    void init_memory_view();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
