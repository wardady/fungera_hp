#ifndef FUNGERA_MEMORY_H
#define FUNGERA_MEMORY_H

#include <vector>
#include <stdexcept>
#include <array>
#include "Cell.h"

class Memory {
public:

    const size_t ncollumns, nrows;

    Memory(const Memory &) = delete;

    void operator=(const Memory &) = delete;

    Memory(size_t ncollumns, size_t nrows, double memory_full_ratio);


    Cell &operator()(size_t x, size_t y);

    bool time_to_kill();

    int is_allocated_region(const std::array<size_t, 2> &instruction_pointer,
                            const std::array<size_t, 2> &size);

    void allocate(const std::array<size_t, 2> &instruction_pointer,
                  const std::array<size_t, 2> &size);

    void deallocate(const std::array<size_t, 2> &instruction_pointer,
                    const std::array<size_t, 2> &size);

private:
    void set_region_allocation(const std::array<size_t, 2> &ip,
                               const std::array<size_t, 2> &size,
                               bool value);

    std::vector<Cell> memory_block;
    double memory_full_ratio;
};


#endif //FUNGERA_MEMORY_H
