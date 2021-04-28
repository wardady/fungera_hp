#ifndef FUNGERA_COMMANDHEATMAP_H
#define FUNGERA_COMMANDHEATMAP_H

#include <vector>
#include <boost/serialization/access.hpp>
#include <boost/serialization/vector.hpp>

class CommandHeatMap {
    friend class boost::serialization::access;

    template<typename Archive>
    void serialize(Archive &ar, unsigned int version) {
        ar & BOOST_SERIALIZATION_NVP(container);
        ar & BOOST_SERIALIZATION_NVP(nrows);
        ar & BOOST_SERIALIZATION_NVP(ncollumns);
    }

    std::vector<size_t> container;
public:

    CommandHeatMap(size_t nrows, size_t ncollumns);

    size_t nrows, ncollumns;

    size_t &operator()(size_t row, size_t column);

};


#endif //FUNGERA_COMMANDHEATMAP_H
