#include "Organism.h"

bool Organism::operator<(const Organism &rhs) const {
    return errors < rhs.errors;
}

