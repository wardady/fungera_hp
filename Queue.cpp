// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include <iostream>
#include "Queue.h"

#include <algorithm>
#include <cassert>

void Queue::kill_organisms() {
    std::sort(organisms.rbegin(), organisms.rend());
    auto ratio = static_cast<size_t>(organisms.size() * kill_organisms_ratio);
    organisms.erase(organisms.begin(), organisms.begin() + ratio);
}

void Queue::push_organism(const Organism &rhs) {
    organisms.emplace_back(rhs);
}

void Queue::push_organism(Organism &&rhs) {
    organisms.emplace_back(std::move(rhs));
}

void Queue::cycle_all() {
    static int org = 0;
    if (organisms.size() > org) {
        org = organisms.size();
        std::cout << org << std::endl;
    }
    for (auto &organism:organisms) {
        organism.cycle();
    }
}

void Queue::remove_organism(Organism &dead) {
    assert( std::find(organisms.begin(), organisms.end(), dead) != organisms.end() );
    organisms.erase(std::find(organisms.begin(), organisms.end(), dead));
}
