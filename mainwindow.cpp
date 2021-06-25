// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QHBoxLayout>
#include <QHeaderView>
#include <QDoubleSpinBox>
#include <QLabel>
#include <QtConcurrent/QtConcurrentRun>

void setup_stats_table(QTableWidget *tbl, int stack_length) {
    tbl->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tbl->verticalHeader()->setVisible(false);
    tbl->horizontalHeader()->setVisible(false);
    QVector<QString> properties = {"Cycle", "Alive", "Purges", "Organism_id",
                                   "    Errors", "    IP", "    Delta", "    RA",
                                   "    RB", "    RC", "    RD"};
    for (int i{0}; i < stack_length; ++i) {
        properties.append(QString("    Stack_").append(QString::number(i)));
    }
    properties.append("Begin");
    properties.append("Size");
    tbl->setColumnCount(2);
    tbl->setRowCount(properties.size());
    for (auto i{0}; i < tbl->rowCount(); ++i) {
        tbl->setItem(i, 0, new QTableWidgetItem(properties[i]));
        tbl->setItem(i, 1, new QTableWidgetItem("0"));
    }
    tbl->horizontalHeader()->setStretchLastSection(true);
}

void MainWindow::init_memory_view() {
    ui->memory_view->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->memory_view->setColumnCount(simulation->config.memory_size[0]); //-V107
    ui->memory_view->setRowCount(simulation->config.memory_size[1]); //-V107
    ui->memory_view->verticalHeader()->setDefaultSectionSize(15);
    ui->memory_view->horizontalHeader()->setDefaultSectionSize(15);
    ui->memory_view->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    ui->memory_view->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    ui->memory_view->setShowGrid(false);

    //! TODO: implement custom data model
    QStringList headerLabels;
    for (int i = 0; i < ui->memory_view->rowCount(); ++i) {
        headerLabels << QString::number(i);
    }
    ui->memory_view->setVerticalHeaderLabels(headerLabels);
    headerLabels.clear();
    for (int i = 0; i < ui->memory_view->rowCount(); ++i) {
        //! TODO: refactor using QTableWidgetItem and inheriting QLabel, smth. like https://programmersought.com/article/13925713867/
        headerLabels << QString::number(i).split("", QString::SkipEmptyParts).join("\n");
        // Future Qt uses Qt::SkipEmptyParts instead of QString::SkipEmptyParts
    }
    ui->memory_view->setHorizontalHeaderLabels(headerLabels);
    for (int i = 0; i < ui->memory_view->rowCount(); ++i) {
        ui->memory_view->horizontalHeaderItem(i)->setTextAlignment(Qt::AlignBottom);
    }

    for (size_t i = 0; i < simulation->config.memory_size[1]; ++i) {
        for (size_t j = 0; j < simulation->config.memory_size[0]; ++j) {
            auto memory_cell = simulation->memory(j, i);
            auto memory_view_item = new QTableWidgetItem(
                    QString(memory_cell.instruction));
            memory_view_item->setTextAlignment(Qt::AlignCenter);
            ui->memory_view->setItem(i, j,memory_view_item); //-V107
            if (!memory_cell.free)
                ui->memory_view->item(i, j)->setBackground(Qt::lightGray); //-V107
        }
    }
}

void MainWindow::setup_gui() {
    for(const auto &instruciton:Organism::instructions){
        ui->break_command_selector->addItem(QString(instruciton.first));
    }
    //ui->organism_selector->addItem("0"); -- set in UI
    setup_stats_table(ui->simulation_stats, simulation->config.stack_length);
    init_memory_view();

    auto control_layout = new QVBoxLayout;
    auto button_layout = new QHBoxLayout;
    auto selected_org_layout = new QHBoxLayout;
    auto stop_ip_in_cell_layout = new QHBoxLayout;
    auto stop_on_command_layout = new QHBoxLayout;
    //stop_ip_in_cell_layout->addWidget(stop_on_ip_in_cell_btn);
    control_layout->addLayout(selected_org_layout);
    control_layout->addLayout(stop_on_command_layout);
    control_layout->addLayout(stop_ip_in_cell_layout);
    control_layout->addLayout(button_layout);
    ui->current_org_pane->setLayout(control_layout); //, 20);
}

const Organism &MainWindow::get_selected_organism() {
    auto org = simulation->get_organism(selected_organism_idx);
    if (org)
        return *org;

    for (size_t i = selected_organism_idx; i < Organism::get_total_organism_num(); ++i) {
        org = simulation->get_organism(i);
        if (org) {
            selected_organism_idx = i;
            return *org;
        }
    }

    for (size_t i = selected_organism_idx; i > 0; --i) {
        org = simulation->get_organism(i - 1);
        if (org) {
            selected_organism_idx = i;
            return *org;
        }
    }
    throw std::out_of_range("No organism was found");
}

MainWindow::MainWindow(Fungera *simulation, QWidget *parent)
        : QMainWindow(parent), simulation(simulation), ui(new Ui::MainWindow),
          selected_organism_idx{},check_for_ip{},stop_ip_coords{},check_for_command{} {
    ui->setupUi(this);
    setup_gui();

    simulation_thread = new QThread;
    simulation->moveToThread(simulation_thread);

    connect(simulation_thread, &QThread::started, simulation, &Fungera::run);

    prev_ip_ptr_m = get_selected_organism().get_ip();
    //! Купу коду лише через одну дрібну красивість :=) TODO: Abstract away.
    if (get_selected_organism().is_ip_on_border())
        prev_ip_brush_m = QBrush{selected_organism_border_color};
    else if (get_selected_organism().is_ip_within())
        prev_ip_brush_m = QBrush{selected_organism_color};
    else
        prev_ip_brush_m = QBrush{nonorganism_color};
    fungera_state_to_view("0");
    connect(simulation, &Fungera::cycle_changed, this,
            &MainWindow::fungera_state_to_view,
            Qt::BlockingQueuedConnection);

    connect(&(simulation->memory), &Memory::memory_cell_changed, ui->memory_view,
            [this](quint64 x, quint64 y, char value, bool free) {
                auto changed_cell = ui->memory_view->item(y, x);
                auto brush = changed_cell->background();
                if (free) {
                    brush.setStyle(Qt::SolidPattern);
                    brush.setColor(nonorganism_color);
                } else {
                    brush.setStyle(Qt::CrossPattern);
                    brush.setColor(nonorganism_allocated_color);
                }
                // changed_cell->setBackground(brush); // TODO: Implement efficently
                changed_cell->setText(QString(value));
                //auto old_color = memory_view->item(y,x)->background();
                //auto memory_cell = new QTableWidgetItem(QString(value));
                //memory_cell->setTextAlignment(Qt::AlignCenter);
                //memory_cell->setBackground(old_color);
                //memory_view->setItem(y,x, memory_cell);
            },
            Qt::BlockingQueuedConnection); // Qt::BlockingQueuedConnection does not work here because of free cells update

    ui->simulation_stats->item(1, 1)->setText(
            QString::number(simulation->get_organisms_num()));
    connect(simulation, &Fungera::alive_changed, ui->simulation_stats,
            [this](quint64 num_alive) {
                ui->simulation_stats->item(1, 1)->setText(
                        QString::number(num_alive));
            }, Qt::BlockingQueuedConnection);
    connect(simulation, &Fungera::purges_changed, ui->simulation_stats,
            [this](quint64 num_purges) {
                ui->simulation_stats->item(2, 1)->setText(
                        QString::number(num_purges));
            }, Qt::BlockingQueuedConnection);

    connect(ui->runall_stopall_btn, &QPushButton::clicked, simulation,
            &Fungera::toggle_simulaiton,
            Qt::DirectConnection); //! Важливо, що маніпулює лише atomic<int>.

    connect(ui->runall_stopall_btn, &QPushButton::clicked, this,
            [this]() {
                if (this->simulation->is_running()) {
                    ui->runall_stopall_btn->setText(tr("Pause All"));
                } else {
                    ui->runall_stopall_btn->setText(tr("Run All"));
                }
            },
            Qt::DirectConnection); //! Важливо, що маніпулює лише atomic<int>.

    connect(ui->step_all_orgs_btn, &QPushButton::clicked, simulation,
            [this]() {
                if (!this->simulation->is_running())
                    QtConcurrent::run(this->simulation, &Fungera::execute_cycle);
    }, Qt::DirectConnection);

    connect(ui->step_current_org_btn, &QPushButton::clicked, this,
            [this]() {
                if (!this->simulation->is_running()) {
                    QtConcurrent::run([this]() {
                        this->simulation->execute_organism(selected_organism_idx);
                    });
                }
            });
    connect(ui->next_org_btn, &QPushButton::clicked, this, [this]() {
        if (selected_organism_idx < Organism::get_total_organism_num())
            selected_organism_idx++;
        scroll_to_current_organism();
        update_organisms_view();
    });
    connect(ui->prev_org_btn, &QPushButton::clicked, this, [this]() {
        if (selected_organism_idx > 0)
            selected_organism_idx--;
        scroll_to_current_organism();
        update_organisms_view();
    });
    connect(simulation, &Fungera::alive_ids, ui->organism_selector,
            [this](QVector<size_t> organisms) {
                std::sort(organisms.begin(), organisms.end());
                ui->organism_selector->clear();
                for (const auto &id:organisms) {
                    ui->organism_selector->addItem(QString::number(id));
                }
            }, Qt::BlockingQueuedConnection);
    connect(ui->organism_selector, &QComboBox::currentTextChanged, this,
            [this](const QString &text) {
                selected_organism_idx = text.toULongLong();
                update_organisms_view();
                scroll_to_current_organism();
            }, Qt::DirectConnection);
//    connect(advance_input, &QLineEdit::returnPressed, this,
//            [this]() {
//                advance_btn->click();
//            });
    connect(ui->step_N_orgs_btn, &QPushButton::clicked, this,
            [this]() {
                if (!this->simulation->is_running()) {
                    QtConcurrent::run([this]() {
                        for (int i = 0; i < ui->step_N_orgs_spinbox->value(); ++i)
                            this->simulation->execute_cycle();
                    });
                }
            });
    connect(ui->step_N_org_btn, &QPushButton::clicked, this,
            [this]() {
                if (!this->simulation->is_running()) {
                    QtConcurrent::run([this]() {
                        for (int i = 0; i < ui->step_N_org_spinbox->value(); ++i)
                            this->simulation->execute_organism(selected_organism_idx);
                    });
                }
            });

    ui->break_ip_x_input->setMaximum(simulation->memory.ncollumns);
    ui->break_ip_y_input->setMaximum(simulation->memory.nrows);
    connect(ui->stop_on_ip_in_cell_btn, &QPushButton::clicked, this,
            [this](){
            stop_ip_coords[0]=ui->break_ip_x_input->text().toULongLong();
            stop_ip_coords[1]=ui->break_ip_y_input->text().toULongLong();
            check_for_ip=true;
    });
    connect(ui->memory_view, QTableWidget::cellDoubleClicked, this,
            [this](int row, int col){
                ui->break_ip_x_input->setValue(col);
                ui->break_ip_y_input->setValue(row);
               }
    );

    ui->mutation_on_reproduction_rate_spinbox->setValue(simulation->config.mutation_on_reproduction_rate);
    // https://stackoverflow.com/questions/16794695/connecting-overloaded-signals-and-slots-in-qt-5
    connect(ui->mutation_on_reproduction_rate_spinbox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this,
            [this](double val){
                this->simulation->config.mutation_on_reproduction_rate = val;
    });


    connect(ui->stop_on_opcode_in_cell_btn, &QPushButton::clicked, this,
            [this](){
        check_for_command = true;
    });
    update_organisms_view();
    scroll_to_current_organism();

    simulation_thread->start();
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::scroll_to_current_organism() {
    const auto &organism = get_selected_organism();
    auto organism_start = organism.get_start();
    ui->memory_view->scrollToItem(
            ui->memory_view->item(organism_start[0], organism_start[1]), //-V107
            QAbstractItemView::PositionAtCenter);
}

void MainWindow::fungera_state_to_view(QString cycle) {
    ui->simulation_stats->item(0, 1)->setText(cycle);

    if(check_for_ip){
        auto new_organism = simulation->organism_ip_in_cell(stop_ip_coords);
        if(new_organism.has_value()){
            selected_organism_idx = new_organism.value();
            check_for_ip = false;
            simulation->toggle_simulaiton();
        }
    }

    auto &selected_organism = get_selected_organism();

    std::array<size_t, 2> instruction_ptr = selected_organism.get_ip();
    auto begin = selected_organism.get_start();
    auto size = selected_organism.get_size();

    ui->simulation_stats->item(3, 1)->setText(
            QString::number(selected_organism.get_id()));
    ui->simulation_stats->item(4, 1)->setText(
            QString::number(selected_organism.get_errors()));
    ui->simulation_stats->item(5, 1)->setText(reg_to_QString(instruction_ptr));
    ui->simulation_stats->item(6, 1)->setText(
            reg_to_QString(selected_organism.get_delta()));
    auto regs = selected_organism.get_registers();
    for (size_t i = 0; i < regs.size(); ++i) {
        ui->simulation_stats->item(7 + i, 1)->setText(
                reg_to_QString(regs.at_index(i)));
    }
    for (int index = 0; const auto &element: selected_organism.get_stack()) {
        ui->simulation_stats->item(11 + index, 1)->setText(reg_to_QString(element));
        ++index;
    }
    ui->simulation_stats->item(19, 1)->setText(reg_to_QString(begin));
    ui->simulation_stats->item(20, 1)->setText(reg_to_QString(size));

    ui->memory_view->item(prev_ip_ptr_m[1], prev_ip_ptr_m[0])->setBackground(
            prev_ip_brush_m);
    prev_ip_brush_m = ui->memory_view->item(instruction_ptr[1],
                                        instruction_ptr[0])->background();
    ui->memory_view->item(instruction_ptr[1],
                      instruction_ptr[0])->setBackground(Qt::red);
    prev_ip_ptr_m = instruction_ptr;

    if(check_for_command){
        if(ui->memory_view->item(instruction_ptr[1],instruction_ptr[0])->text()
                == ui->break_command_selector->currentText()){
            simulation->toggle_simulaiton();
            check_for_command=false;
        }
    }

}

void MainWindow::update_organisms_view() {
    // Quick and dirty...
    for (const auto &org: simulation->queue.get_container()) {
        set_organism_color(org, organism_color, organism_border_color);
    }
    auto &selected_organism = get_selected_organism();
    set_organism_color(selected_organism, selected_organism_color,
                       selected_organism_border_color);
}

void MainWindow::set_organism_color(const Organism &organism, const QColor &color,
                                    const QColor &border_color) {
    auto beg_row = organism.get_start()[1];
    auto fin_row = organism.get_start()[1] + organism.get_size()[1];
    auto beg_col = organism.get_start()[0];
    auto fin_col = organism.get_start()[0] + organism.get_size()[0];

    for (size_t row = beg_row; row < fin_row; ++row) {
        for (size_t col = beg_col; col < fin_col; ++col) {
            if ((row == beg_row || row == fin_row - 1) ||
                (col == beg_col || col == fin_col - 1))
                ui->memory_view->item(row, col)->setBackground(
                        border_color); // Градієнтом зробити, чи що... //-V107
            else
                ui->memory_view->item(row, col)->setBackground(color); //-V107
        }
    }
}
