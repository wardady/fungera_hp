#ifndef FUNGERA_CONFIG_H
#define FUNGERA_CONFIG_H

#include <string>
#include <array>

struct Config {
    // TODO: implement config parser
    std::array<size_t, 2> memory_size{5000, 5000};
    int random_rate = 5;
    size_t cycle_gap = 10000;
    double memory_full_ratio = 0.9, kill_organisms_ratio = 0.5;

    explicit Config(const std::string &path) = default;

};

#endif //FUNGERA_CONFIG_H
