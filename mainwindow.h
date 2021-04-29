#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QTableWidget>
#include <QTextBrowser>
#include <QThread>

#include"Fungera.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

    QWidget *central_window;
    QPushButton *toggle_btn, *cycle_btn;
    QTableWidget *simulation_stats;
    QTextBrowser *memory_view;
    QThread *simulation_thread;
    Fungera *simulation;



public:
    MainWindow(Fungera *simulation,QWidget *parent = nullptr);
    ~MainWindow();

    void setup_gui();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
