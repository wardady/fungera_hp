#ifndef FUNGERA_FUNGERA_H
#define FUNGERA_FUNGERA_H


#include <curses.h>
#include <string>
#include <random>
#include <boost/multiprecision/cpp_int.hpp>
#include "Memory.h"
#include "Queue.h"
#include "Config.h"
#include <fstream>

namespace mp = boost::multiprecision;

class Fungera {
public:
    Fungera(const Fungera &) = delete;

    Fungera &operator=(const Fungera &) = delete;

    explicit Fungera(const std::string &config_path);

    ~Fungera() = default;

    void run();

    void execute_cycle();

private:
    mp::checked_uint1024_t cycle;
//    bool is_running;
//    SCREEN *screen;
    Config config;
    Memory memory;
    Queue queue;
    size_t purges;
    std::discrete_distribution<int> radiation_dist;
    std::ofstream geneaology_log;

    void load_initial_genome(const std::string &filename,
                             const std::array<size_t, 2> &address);

    void radiation();

    void info_log();

    void new_child_log();
};

#endif //FUNGERA_FUNGERA_H
