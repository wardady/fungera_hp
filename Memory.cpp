// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include <random>
#include "Memory.h"

Memory::Memory(size_t ncollumns, size_t nrows, double memory_full_ratio) : memory_full_ratio{memory_full_ratio}, ncollumns{ncollumns},
                                                                           nrows{nrows} {
    memory_block.reserve(ncollumns * nrows);
}

void Memory::radiation() {
    // TODO:implement
    return;
}

bool Memory::time_to_kill() {
    size_t nfree = 0, nbusy = 0;
    for (const auto &cell:memory_block) {
        if (cell.free)
            nfree++;
        else
            nbusy++;
    }
    return (static_cast<double>(nbusy) / nfree) > memory_full_ratio;
}
