// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <stdexcept>
#include "CommandHeatMap.h"

size_t &CommandHeatMap::operator()(size_t row, size_t column) {
    if (column > ncollumns || row > nrows)
        throw std::out_of_range("Index error");
    return container[row * nrows + column];
}

CommandHeatMap::CommandHeatMap(size_t nrows,
                               size_t ncollumns) : nrows{nrows},
                                                   ncollumns{ncollumns},
                                                   container(
                                                           (nrows * ncollumns) +
                                                           1, 0) {
}
