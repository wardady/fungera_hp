// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include <random>
#include "Memory.h"

Memory::Memory(size_t ncollumns, size_t nrows, double memory_full_ratio)
        : memory_full_ratio{memory_full_ratio}, ncollumns{ncollumns},
          nrows{nrows}, memory_block{nrows * ncollumns} {
}

bool Memory::time_to_kill() {
    size_t nfree = 0, nbusy = 0;
    for (const auto &cell:memory_block) {
        if (cell.free)
            nfree++;
        else
            nbusy++;
    }
    return (static_cast<double>(nbusy) / static_cast<double>(nfree)) > memory_full_ratio;
}

void Memory::set_region_allocation(const std::array<size_t, 2> &ip,
                                   const std::array<size_t, 2> &size,
                                   bool value) {

    for (size_t i = ip[1]; i < ip[1] + size[1]; ++i)
        for (size_t j = ip[0]; j < ip[0] + size[0]; ++j) {
            (*this)(i, j).free = value;
            emit memory_cell_changed(j, i, (*this)(i, j).instruction, value); //! Deadlock here if BlockingQueuedConnection is used
        }
}

void Memory::allocate(const std::array<size_t, 2> &instruction_pointer,
                      const std::array<size_t, 2> &size) {
    set_region_allocation(instruction_pointer, size, false);
}

void Memory::deallocate(const std::array<size_t, 2> &instruction_pointer,
                        const std::array<size_t, 2> &size) {
    set_region_allocation(instruction_pointer, size, true);
}

Cell& Memory::operator()(size_t x, size_t y) {
    if (x >= ncollumns || y >= nrows) // TODO: Перевірити, чи не "транспонується" воно десь -- чи немає викликів
                                        // (row, col) -- як для QTableView-ів.
        throw std::out_of_range("Index error");
    if(y == 500 && (x >= 500 || x<=523)){
        volatile auto xx = x + y;
    }
    return memory_block[y * nrows + x];
}

const Cell& Memory::operator()(size_t x, size_t y) const{
    if (x >= ncollumns || y >= nrows)
        throw std::out_of_range("Index error");
    return memory_block[y * nrows + x];
}
Cell& Memory::operator()(const std::array<size_t, 2>& coord){
    return operator()(coord[0], coord[1]);
}
const Cell& Memory::operator()(const std::array<size_t, 2>& coord) const{
    return operator()(coord[0], coord[1]);
}

int
Memory::is_allocated_region(const std::array<size_t, 2> &instruction_pointer,
                            const std::array<size_t, 2> &size) {
    if (instruction_pointer[1] * nrows + size[0] + instruction_pointer[0] >=
        memory_block.size())
        // TODO: Consider error?
        //throw std::out_of_range("Region is out of bounds of memory_ptr_m block");
        return -1;
    for (size_t i = instruction_pointer[1]; i < instruction_pointer[1] + size[1]; ++i)
        for (size_t j = instruction_pointer[0]; j < instruction_pointer[0] + size[0]; ++j)
            if (! (*this)(j, i).free )
                return 1;
    return 0;
}

void Memory::set_cell_value(size_t x, size_t y, char value) {
    if (x >= ncollumns || y >= nrows)
        throw std::out_of_range("Index error");
    memory_block[y * nrows + x].instruction = value;
    emit memory_cell_changed(x, y, value, memory_block[y * nrows + x].free);
}

