// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "Fungera.h"
#include <iostream>
#include <fstream>

Fungera::Fungera(const std::string &config_path) : config{config_path},
                                                   memory{config.memory_size[0],
                                                          config.memory_size[1],
                                                          config.memory_full_ratio},
                                                   queue{config.kill_organisms_ratio},
                                                   purges{}, cycle{} {
    load_initial_genome("../initial.gen",
                        {config.memory_size[0] / 2, config.memory_size[1] / 2});
}

void Fungera::load_initial_genome(const std::string &filename,
                                  const std::array<size_t, 2> &address) {
    std::ifstream infile{filename};
    std::string data((std::istreambuf_iterator<char>(infile)),
                     std::istreambuf_iterator<char>());
    size_t row = address[1];
    size_t column = address[0];
    for (const auto &op_code:data) {
        if (op_code == '\n') {
            row++;
            column = address[0];
        } else {
            memory(column, row).instruction = op_code;
            memory(column, row).free = false;
            column++;
        }
    }

    queue.push_organism(Organism(
            std::array<size_t, 2>{data.find('\n'),
                                  static_cast<size_t>(std::count(data.begin(),
                                                                 data.end(),
                                                                 '\n'))},
            address, address,
            &memory, &queue, &config));
}

void Fungera::run() {
    while (!queue.empty()) {
        execute_cycle();
    }
}


void Fungera::execute_cycle() {
    queue.cycle_all();
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
