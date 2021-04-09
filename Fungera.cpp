// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "Fungera.h"
#include <iostream>
#include <fstream>
#include <random>

#include "common.h"

Fungera::Fungera(const std::string &config_path) : config{config_path},
                                                   memory{config.memory_size[0],
                                                          config.memory_size[1],
                                                          config.memory_full_ratio},
                                                   queue{config.kill_organisms_ratio},
                                                   purges{}, cycle{0} {
    load_initial_genome("../initial.gen",
                        {config.memory_size[0] / 2, config.memory_size[1] / 2});

    double harmonic = 0, prob_of_rad =
            config.radiation_mutation_rate * 100, prob;
    for (int i{1}; i <= config.max_num_of_mutations_rad; ++i) {
        harmonic += 1.0 / i;
    }
    prob = prob_of_rad / harmonic;
    std::vector<int> num_of_rad_probs(config.max_num_of_mutations_rad + 1);
    std::generate(num_of_rad_probs.begin(), num_of_rad_probs.end(),
                  [n = 0, prob, prob_of_rad]()mutable {
                      if (n == 0) {
                          ++n;
                          return 100 - prob_of_rad;
                      } else {
                          return prob / n++;
                      }
                  });
    radiation_dist = std::discrete_distribution<>(num_of_rad_probs.begin(),
                                                  num_of_rad_probs.end());
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


void Fungera::radiation() {
    static std::random_device rd;
    static std::mt19937 gen{rd()};

    for (int i{0}; i < radiation_dist(gen); ++i) {
        memory(fungera::random(static_cast<size_t>(0), memory.ncollumns),
               fungera::random(static_cast<size_t>(0),
                               memory.nrows)).instruction = std::next(
                Organism::instructions.begin(),
                fungera::random(static_cast<size_t>(0),
                                Organism::instructions.size()))->first;
    }
}


void Fungera::execute_cycle() {
    queue.cycle_all();
    radiation();
    if (cycle % config.cycle_gap == 0) {
        if (memory.time_to_kill()) {
            queue.kill_organisms();
            purges++;
        }
    }
    cycle++;
}
