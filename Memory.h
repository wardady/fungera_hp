#ifndef FUNGERA_MEMORY_H
#define FUNGERA_MEMORY_H

#include <vector>
#include "Cell.h"

class Memory {
public:
    Memory(const Memory &) = delete;

    void operator=(const Memory &) = delete;

    Memory(size_t ncollumns, size_t nrows, const double memory_full_ratio);

    void radiation();

    bool time_to_kill();

private:
    size_t ncollumns, nrows;
    std::vector<Cell> memory_block;
    const double memory_full_ratio;
};


#endif //FUNGERA_MEMORY_H
