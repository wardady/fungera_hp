#ifndef FUNGERA_COMMANDHEATMAP_H
#define FUNGERA_COMMANDHEATMAP_H

#include <vector>

class CommandHeatMap {
    std::vector<size_t> container;
public:

    CommandHeatMap(size_t nrows, size_t ncollumns);

    size_t nrows, ncollumns;

    size_t &operator()(size_t row, size_t column);
};


#endif //FUNGERA_COMMANDHEATMAP_H
