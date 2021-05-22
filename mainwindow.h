#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QTableWidget>
#include <QTextBrowser>
#include <QThread>
#include <QColor>
#include <QComboBox>
#include <QLineEdit>

#include "Fungera.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE


class MainWindow : public QMainWindow
{
    Q_OBJECT

    QWidget *central_window;
    QPushButton *toggle_btn, *cycle_btn,*next_btn,*prev_btn, *advance_btn, *selected_org_cycle;
    QLineEdit *advance_input;
    QTableWidget *simulation_stats;
    QTableWidget *memory_view;
    QThread *simulation_thread;
    QComboBox *organism_selector;
    Fungera *simulation;
    size_t selected_organism_idx;

public:
    MainWindow(Fungera *simulation,QWidget *parent = nullptr);
    ~MainWindow();

    void setup_gui();

    void scroll_to_current_organism(); // Should be public as a hack -- called before show() does not have desirable effect.

protected:
    const Organism &get_selected_organism();
    void init_memory_view();
    void fungera_state_to_view(QString cycle);
    void update_organisms_view();
    void set_organism_color(const Organism& organism, const QColor& color, const QColor& border_color);

    QColor selected_organism_color{Qt::blue};
    QColor selected_organism_border_color {Qt::cyan };
    QColor organism_color{Qt::gray};
    QColor organism_border_color { Qt::darkGray };
    QColor nonorganism_color{Qt::white};
    QColor nonorganism_allocated_color{Qt::lightGray};
private:
    Ui::MainWindow *ui;

    std::array<size_t, 2> prev_ip_ptr_m;
    QBrush prev_ip_brush_m{nonorganism_color};
};

#endif // MAINWINDOW_H
