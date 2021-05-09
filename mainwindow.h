#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QTableWidget>
#include <QTextBrowser>
#include <QThread>
#include <QColor>

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

    void scroll_to_current_organism(); // Should be public as a hack -- called before show() does not have desirable effect.

protected:
    const Organism &get_selected_organism();
    void init_memory_view();
    void fungera_state_to_view(QString cycle);
    void set_organism_color(const Organism& organism, QColor color, QColor border_color);

    QColor selected_organism_color{Qt::blue};
    QColor selected_organism_border_color {Qt::cyan };
    QColor organism_color{Qt::gray};
    QColor organism_border_color { Qt::darkGray };
    QColor nonorganism_color{Qt::white};
private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
