// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include <iostream>
#include "Queue.h"

#include <algorithm>
#include <cassert>

void Queue::kill_organisms() {
    organisms.sort();
    auto ratio = static_cast<size_t>( static_cast<double>(organisms.size()) * kill_organisms_ratio);
    auto threshold_it = organisms.begin();
    std::advance(threshold_it, organisms.size() - ratio);
    organisms.erase(threshold_it, organisms.end());
}

void Queue::push_organism(const Organism &rhs) {
    organisms.emplace_back(rhs);
}

void Queue::push_organism(Organism &&rhs) {
    organisms.emplace_back(std::move(rhs));
}

void Queue::cycle_all() {
    for (auto it{organisms.begin()}; it != organisms.end();) {
        auto new_it = it->cycle();
        if (new_it)
            it = new_it.value();
        else
            it++;
    }
}

size_t Queue::size() const {
    return organisms.size();
}

std::list<Organism>::iterator Queue::remove_organism(Organism &dead) {
    assert(std::find(organisms.begin(), organisms.end(), dead) !=
           organisms.end());
    return organisms.erase(std::find(organisms.begin(), organisms.end(), dead));
}

const std::list<Organism> &Queue::get_container() const {
    return organisms;
}

std::list<Organism> &Queue::get_container() {
    return organisms;
}
