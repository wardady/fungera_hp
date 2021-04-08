// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "Fungera.h"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <filesystem>
#include <random>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/multiprecision/cpp_int/serialize.hpp>

#include "common.h"

namespace fs = std::filesystem;

Fungera::Fungera(const std::string &config_path) : config{config_path},
                                                   memory{config.memory_size[0],
                                                          config.memory_size[1],
                                                          config.memory_full_ratio},
                                                   queue{config.kill_organisms_ratio},
                                                   purges{}, cycle{0} {
    load_initial_genome("../initial.gen",
                        {config.memory_size[0] / 2, config.memory_size[1] / 2});
    geneaology_log.open("genealogy_log.log");
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

void Fungera::info_log() {
    std::cout << "[LOG]: " << "Cycle: " << cycle
              << " Number of organisms: " << queue.size() << std::endl;
}

void Fungera::new_child_log() {
    std::cout << "[LOG]: " << "Cycle: " << cycle
              << " Number of organisms: " << queue.size() << std::endl;
    geneaology_log << "Cycle: " << cycle << std::endl;
    for (const auto &organism:queue.get_container()) {
        geneaology_log << "\tOrganism: " << organism.get_id() << std::endl;
        geneaology_log << "\t\tParent: " << organism.get_parent() << std::endl;
        geneaology_log << "\t\tChildren: ";
        std::copy(organism.get_children().begin(),
                  organism.get_children().end(),
                  std::ostream_iterator<size_t>(geneaology_log, ", "));
        geneaology_log << std::endl;
    }
    geneaology_log << std::endl;
}

void Fungera::run() {
    static size_t organism_num = 0;
    while (!queue.empty()) {
        if (organism_num != queue.size()) {
            if (organism_num < queue.size())
                new_child_log();
            else
                info_log();
            organism_num = queue.size();
        } else if (cycle % config.cycle_gap == 0) {
            info_log();
        }
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

void Fungera::save_snapshot() {
    std::cout << "[LOG]: Saving a snapshot on cycle: " << cycle << std::endl;
    fs::create_directories("snapshots");
    std::stringstream snapshot_file{};
    snapshot_file << "snapshots/snapshot_" << cycle << ".txt";
    std::ofstream ofs(snapshot_file.str(), std::ios::trunc);
    assert(ofs.good());
    boost::archive::text_oarchive oa(ofs);
    oa << boost::serialization::make_nvp("Fungera", *this);
}

void Fungera::load_from_snapshot(const std::string &path) {
    std::cout << "[LOG]: Loading from snapshot on cycle: " << std::flush;
    std::ifstream ifs(path);
    assert(ifs.good());
    boost::archive::text_iarchive ia(ifs);
    ia >> *this;
    std::cout << cycle << std::endl;
}


void Fungera::execute_cycle() {
    if (cycle % config.cycle_gap == 0) {
        if (memory.time_to_kill()) {
            queue.kill_organisms();
            purges++;
        }
    }
    if (cycle % config.snapshot_rate == 0) {
        save_snapshot();
    }
    queue.cycle_all();
    radiation();
    cycle++;
}

Fungera::Fungera() : memory{0, 0, 0}, config{""}, queue{0} {

}

