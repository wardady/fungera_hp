// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "Organism.h"

bool Organism::operator<(const Organism &rhs) const {
    return errors < rhs.errors;
}

