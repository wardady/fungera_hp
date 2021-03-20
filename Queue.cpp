// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "Queue.h"

void Queue::kill_organisms() {
    std::sort(organisms.rbegin(), organisms.rend());
    auto ratio = static_cast<size_t>(organisms.size() * kill_organisms_ratio);
    organisms.erase(organisms.begin(), organisms.begin() + ratio);
}

void Queue::emplace_organism(Organism &rhs) {
    organisms.emplace_back(rhs);
}

void Queue::emplace_organism(Organism &&rhs) {
    organisms.emplace_back(std::move(rhs));
}

void Queue::cycle_all() {
    for (auto &organism:organisms) {
        organism.cycle();
    }
}
