// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QHBoxLayout>
#include <QHeaderView>
#include <QtConcurrent/QtConcurrentRun>

void setup_stats_table(QTableWidget *tbl, int stack_length) {
    tbl->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tbl->verticalHeader()->setVisible(false);
    tbl->horizontalHeader()->setVisible(false);
    QVector<QString> properties = {"Cycle", "Alive", "Purges", "Organism_id",
                                   "  Errors", "  IP", "  Delta", "  RA",
                                   "  RB", "  RC", "  RD"};
    for (int i{0}; i < stack_length; ++i) {
        properties.append(QString("  Stack_").append(QString::number(i)));
    }
    tbl->setColumnCount(2);
    tbl->setRowCount(properties.size());
    for (auto i{0}; i < tbl->rowCount(); ++i) {
        tbl->setItem(i, 0, new QTableWidgetItem(properties[i]));
        tbl->setItem(i, 1, new QTableWidgetItem("0"));
    }
    tbl->horizontalHeader()->setStretchLastSection(true);
}

void MainWindow::init_memory_view() {
    memory_view->setEditTriggers(QAbstractItemView::NoEditTriggers);
    memory_view->setColumnCount(simulation->config.memory_size[0]);
    memory_view->setRowCount(simulation->config.memory_size[1]);
    memory_view->verticalHeader()->setDefaultSectionSize(15);
    memory_view->horizontalHeader()->setDefaultSectionSize(15);
    memory_view->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    memory_view->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    memory_view->setShowGrid(false);
    for (size_t i{0}; i < simulation->config.memory_size[1]; ++i) {
        for (size_t j{0}; j < simulation->config.memory_size[0]; ++j) {
            auto memory_cell = simulation->memory(j, i);
            auto memory_view_item = new QTableWidgetItem(
                    QString(memory_cell.instruction));
            memory_view_item->setTextAlignment(Qt::AlignCenter);
            memory_view->setItem(i, j, memory_view_item);
            if (!memory_cell.free)
                memory_view->item(i, j)->setBackground(Qt::lightGray);
        }
    }
}

void MainWindow::setup_gui() {
    auto window = new QWidget(this);
    toggle_btn = new QPushButton("Play", this);
    cycle_btn = new QPushButton("Cycle", this);
    next_btn = new QPushButton("Next", this);
    prev_btn = new QPushButton("Prev", this);
    simulation_stats = new QTableWidget(this);
    memory_view = new QTableWidget(this);

    setup_stats_table(simulation_stats, simulation->config.stack_length);
    init_memory_view();

    auto main_layout = new QHBoxLayout;
    auto control_layout = new QVBoxLayout;
    auto button_layout = new QHBoxLayout;
    control_layout->addWidget(simulation_stats);
    button_layout->addWidget(toggle_btn);
    button_layout->addWidget(cycle_btn);
    button_layout->addWidget(next_btn);
    button_layout->addWidget(prev_btn);
    control_layout->addLayout(button_layout);
    main_layout->addLayout(control_layout, 33);
    main_layout->addWidget(memory_view, 66);

    window->setLayout(main_layout);
    setCentralWidget(window);
}

const Organism &MainWindow::get_selected_organism() {
    auto org = simulation->get_organism(selected_organism);
    if (org)
        return org.value();

    for (size_t i{selected_organism}; i < Organism::get_id_seed(); ++i) {
        org = simulation->get_organism(i);
        if (org) {
            selected_organism = i;
            return org.value();
        }
    }

    for (size_t i{selected_organism}; i >= 0; --i) {
        org = simulation->get_organism(i);
        if (org) {
            selected_organism = i;
            return org.value();
        }
    }
    throw std::out_of_range("No organism was found");
}

MainWindow::MainWindow(Fungera *simulation, QWidget *parent)
        : QMainWindow(parent), simulation(simulation), ui(new Ui::MainWindow),
          selected_organism() {
    ui->setupUi(this);
    setup_gui();

    simulation_thread = new QThread;
    simulation->moveToThread(simulation_thread);

    connect(simulation_thread, &QThread::started, simulation, &Fungera::run);
    connect(simulation, &Fungera::cycle_changed, simulation_stats,
            [this](QString cycle) {
                std::array<size_t, 2> instrcution_ptr{};
                simulation_stats->item(0, 1)->setText(cycle);
                auto organism = get_selected_organism();
                for (size_t i{organism.get_start()[1]}; i <
                                                        organism.get_start()[1] +
                                                        organism.get_size()[1]; ++i) {
                    for (size_t j{organism.get_start()[0]}; j <
                                                            organism.get_start()[0] +
                                                            organism.get_size()[0]; ++j) {
                        memory_view->item(i, j)->setBackground(Qt::blue);
                    }
                }
                std::copy(organism.get_ip().begin(), organism.get_ip().end(),
                          instrcution_ptr.begin());
                simulation_stats->item(3, 1)->setText( QString::number(organism.get_id()) );
                simulation_stats->item(4, 1)->setText( QString::number(organism.get_errors()) );
                simulation_stats->item(5, 1)->setText( reg_to_QString(instrcution_ptr) );
                simulation_stats->item(6, 1)->setText( reg_to_QString(organism.get_delta()) );
                auto regs = organism.get_registers();
                for (int i = 0; i < regs.size(); ++i) {
                    simulation_stats->item(7 + i, 1)->setText( reg_to_QString(regs['a' + i]) );
                }
                auto stack = organism.get_stack();
                for (int index = 0; const auto &element: stack){
                    simulation_stats->item(11 + index, 1)->setText( reg_to_QString(element) );
                    ++index;
                }
                memory_view->item(instrcution_ptr[1],
                                  instrcution_ptr[0])->setBackground(Qt::red);
            }, Qt::BlockingQueuedConnection);

    connect(&(simulation->memory), &Memory::memory_cell_changed,memory_view,[this](quint64 x,quint64 y,char value){
        auto old_color = memory_view->item(y,x)->background();
        auto memory_cell = new QTableWidgetItem(QString(value));
        memory_cell->setTextAlignment(Qt::AlignCenter);
        memory_cell->setBackground(old_color);
        memory_view->setItem(y,x, memory_cell);
    },Qt::BlockingQueuedConnection);

    connect(simulation, &Fungera::alive_changed, simulation_stats,
            [this](quint64 num_alive) {
                simulation_stats->item(1, 1)->setText( QString::number(num_alive) );
            }, Qt::BlockingQueuedConnection);
    connect(simulation, &Fungera::purges_changed, simulation_stats,
            [this](quint64 num_purges) {
                simulation_stats->item(2, 1)->setText( QString::number(num_purges) );
            }, Qt::BlockingQueuedConnection);

    connect(toggle_btn, &QPushButton::clicked, simulation,
            &Fungera::toggle_simulaiton, Qt::DirectConnection); //! Важливо, що маніпулює лише atomic<int>.

    connect(cycle_btn, &QPushButton::clicked, simulation, [this]() {
        if (!this->simulation->is_running.load())
            QtConcurrent::run(this->simulation, &Fungera::execute_cycle);
    }, Qt::DirectConnection);
    connect(next_btn, &QPushButton::clicked, this, [this]() {
        if (selected_organism < Organism::get_id_seed())
            selected_organism++;
    });
    connect(prev_btn, &QPushButton::clicked, this, [this]() {
        if (selected_organism > 0)
            selected_organism--;
    });

    simulation_thread->start();
}

MainWindow::~MainWindow() {
    delete ui;
}

