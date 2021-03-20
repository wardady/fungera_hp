// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include <random>
#include "Memory.h"

Memory::Memory(size_t ncollumns, size_t nrows, double memory_full_ratio)
        : memory_full_ratio{memory_full_ratio}, ncollumns{ncollumns},
          nrows{nrows}, memory_block{nrows * ncollumns} {
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

void Memory::set_region_allocation(const std::array<size_t, 2> &ip,
                                   const std::array<size_t, 2> &size,
                                   bool value) {

    for (size_t i{ip[1]};
         i < ip[1] + size[1]; ++i)
        for (size_t j{ip[0]};
             j < ip[0] + size[0]; ++j)
            memory_block[i * nrows + j].free = value;
}

void Memory::allocate(const std::array<size_t, 2> &instruction_pointer,
                      const std::array<size_t, 2> &size) {
    set_region_allocation(instruction_pointer, size, false);
}

void Memory::deallocate(const std::array<size_t, 2> &instruction_pointer,
                        const std::array<size_t, 2> &size) {
    set_region_allocation(instruction_pointer, size, true);
}

Cell &Memory::operator()(size_t x, size_t y) {
    if (x > ncollumns || y > nrows)
        throw std::out_of_range("Index error");
    return memory_block[y * nrows + x];
}

int
Memory::is_allocated_region(const std::array<size_t, 2> &instruction_pointer,
                            const std::array<size_t, 2> &size) {
    if (instruction_pointer[1] * nrows + size[0] + instruction_pointer[0] >
        memory_block.size())
        // TODO: if considered error, throw (probably rework to call for every allocate/deallocate call)
        //throw std::out_of_range("Region is out of bounds of memory block");
        return -1;
    for (size_t i{instruction_pointer[1]};
         i < instruction_pointer[1] + size[1]; ++i)
        for (size_t j{instruction_pointer[0]};
             j < instruction_pointer[0] + size[0]; ++j)
            if (!memory_block[i * nrows + j].free)
                return 1;
    return 0;
}
