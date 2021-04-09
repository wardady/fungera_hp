#ifndef FUNGERA_FUNGERA_H
#define FUNGERA_FUNGERA_H


#include <curses.h>
#include <string>
#include <random>
#include "Memory.h"
#include "Queue.h"
#include "Config.h"

class Fungera {
public:
    Fungera(const Fungera &) = delete;

    Fungera &operator=(const Fungera &) = delete;

    Fungera(const std::string &config_path);

    ~Fungera() = default;

    void run();

    void execute_cycle();

private:
    size_t cycle;
//    bool is_running;
//    SCREEN *screen;
    Config config;
    Memory memory;
    Queue queue;
    size_t purges;
    std::discrete_distribution<int> radiation_dist;

    void load_initial_genome(const std::string &filename,
                             const std::array<size_t, 2> &address);

    void radiation();

};

#endif //FUNGERA_FUNGERA_H
