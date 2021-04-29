#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QHBoxLayout>
#include <QHeaderView>

void setup_stats_table(QTableWidget *tbl){
    QVector<QString> properties = {"Cycle","Alive"};
    tbl->setColumnCount(2);
    tbl->setRowCount(properties.size());
    for(auto i{0};i<tbl->rowCount();++i){
       tbl->setItem(i,0,new QTableWidgetItem(properties[i]));
       tbl->setItem(i,1,new QTableWidgetItem("0"));
    }
    tbl->horizontalHeader()->setStretchLastSection(true);
}

void MainWindow::setup_gui()
{
    auto window = new QWidget(this);
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

MainWindow::MainWindow(Fungera *simulation, QWidget *parent)
    : QMainWindow(parent)
    , simulation(simulation)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setup_gui();

    simulation_thread = new QThread;
    simulation->moveToThread(simulation_thread);

    connect(simulation_thread,&QThread::started,simulation,&Fungera::run);
    connect(simulation,&Fungera::cycle_changed,simulation_stats,[this](QString cycle){
        simulation_stats->setItem(0,1,new QTableWidgetItem(cycle));
    },Qt::BlockingQueuedConnection);
    connect(simulation,&Fungera::alive_changed,simulation_stats,[this](quint64 num_alive){
        simulation_stats->setItem(1,1,new QTableWidgetItem(QString::number(num_alive)));
    });

    simulation_thread->start();
}

MainWindow::~MainWindow()
{
    delete ui;
}

