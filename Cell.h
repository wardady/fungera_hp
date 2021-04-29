#ifndef FUNGERA_CELL_H
#define FUNGERA_CELL_H

#include <boost/serialization/access.hpp>

struct Cell {
    char instruction = '.';
    bool free = true;

    friend class boost::serialization::access;

    template<class Archive>
    void serialize(Archive &ar, unsigned int version) {
        ar & BOOST_SERIALIZATION_NVP(instruction);
        ar & BOOST_SERIALIZATION_NVP(free);
    }
};

#endif //FUNGERA_CELL_H
