#include <random>
#include "Memory.h"

Memory::Memory(size_t ncollumns, size_t nrows, const double memory_full_ratio) : memory_full_ratio{memory_full_ratio}, ncollumns{ncollumns},
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
    return nbusy / nfree > memory_full_ratio;
}
