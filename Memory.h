#ifndef FUNGERA_MEMORY_H
#define FUNGERA_MEMORY_H

#include <vector>
#include <stdexcept>
#include <array>
#include "Cell.h"

class Memory {
public:
//    Memory(const Memory &) = delete;

    void operator=(const Memory &) = delete;

    Memory(size_t ncollumns, size_t nrows, double memory_full_ratio);

    void radiation();

    Cell &operator()(size_t x, size_t y) {
        if (x > ncollumns || y > nrows)
            throw std::out_of_range("Index error");
        return memory_block[y * nrows + x];
    }

    bool time_to_kill();

    int is_allocated_region(std::array<size_t, 2> instruction_pointer,
                            std::array<size_t, 2> size);

    void allocate(std::array<size_t, 2> instruction_pointer,
                  std::array<size_t, 2> size);

private:
    size_t ncollumns, nrows;
    std::vector<Cell> memory_block;
    double memory_full_ratio;
};


#endif //FUNGERA_MEMORY_H
