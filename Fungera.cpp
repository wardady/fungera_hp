// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "Fungera.h"
#include <iostream>

Fungera::Fungera(const std::string &config_path) : config{config_path},
                                                   memory{config.memory_size[0], config.memory_size[1],
                                                          config.memory_full_ratio},queue{config.kill_organisms_ratio} {
    auto fd = fopen("/dev/tty", "r+");
    screen = newterm(nullptr, fd, fd);
    noecho();
    timeout(0);
    keypad(stdscr, true);
}

[[noreturn]] void Fungera::run() {
    decltype(getch()) c; // ВЗагалі кажучи, тут int! -- з ERR були б проблеми. Прочитаєте -- приберіть коментар.
    while (true) {
        // TODO: handle input
        c = getch();
        switch (c) {
            case ' ':
                is_running = !is_running;
                break;
            case ERR:
            default:
                execute_cycle();
                // TODO: visualisation
                break;
        }
        refresh();
    }
}

Fungera::~Fungera() {
    delscreen(screen);
}

void Fungera::execute_cycle() {
    if (cycle % config.random_rate == 0) {
        memory.radiation();
    }
    if (cycle % config.cycle_gap == 0) {
        if (memory.time_to_kill()) {
            queue.kill_organisms();
            purges++;
        }
    }
    cycle++;
}
