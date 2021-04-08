#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QHBoxLayout>
#include <QHeaderView>

void setup_stats_table(QTableWidget *tbl){
    QVector<QString> properties = {"Cycle"};
    tbl->setColumnCount(2);
    tbl->setRowCount(properties.size());
    for(auto i{0};i<tbl->rowCount();++i){
       tbl->setItem(i,0,new QTableWidgetItem(properties[i]));
       tbl->setItem(i,1,new QTableWidgetItem("0"));
    }
    tbl->horizontalHeader()->setStretchLastSection(true);
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    auto window = new QWidget();
    toggle_btn = new QPushButton("Play",this);
    cycle_btn = new QPushButton("Cycle",this);
    simulation_stats = new QTableWidget(this);
    memory_view = new QTextBrowser(this);
    setup_stats_table(simulation_stats);

    auto main_layout = new QHBoxLayout;
    auto control_layout = new QVBoxLayout;
    auto button_layout = new QHBoxLayout;
    control_layout->addWidget(simulation_stats);
    button_layout->addWidget(toggle_btn);
    button_layout->addWidget(cycle_btn);
    control_layout->addLayout(button_layout);
    main_layout->addLayout(control_layout,33);
    main_layout->addWidget(memory_view,66);

    window->setLayout(main_layout);
    setCentralWidget(window);
}

MainWindow::~MainWindow()
{
    delete ui;
}

