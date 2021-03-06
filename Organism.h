#ifndef FUNGERA_ORGANISM_H
#define FUNGERA_ORGANISM_H


#include <cstddef>

class Organism {
public:
    bool operator<(const Organism &rhs) const;

private:
    size_t errors;
};


#endif //FUNGERA_ORGANISM_H
