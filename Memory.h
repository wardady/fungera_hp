#ifndef FUNGERA_MEMORY_H
#define FUNGERA_MEMORY_H

#include <vector>
#include <stdexcept>
#include <array>
#include <QObject>
#include <boost/serialization/access.hpp>
#include <boost/serialization/vector.hpp>
#include "Cell.h"

class Memory : public QObject {
Q_OBJECT

    friend class boost::serialization::access;

    template<class Archive>
    void serialize(Archive &ar, unsigned int version) {
        ar & BOOST_SERIALIZATION_NVP(ncollumns);
        ar & BOOST_SERIALIZATION_NVP(nrows);
        ar & BOOST_SERIALIZATION_NVP(memory_block);
        ar & BOOST_SERIALIZATION_NVP(memory_full_ratio);
    }

public:

    size_t ncollumns, nrows;

    void set_cell_value(size_t x, size_t y, char value);

    Memory(const Memory &) = delete;

    void operator=(const Memory &) = delete;

    Memory(size_t ncollumns, size_t nrows, double memory_full_ratio);


    Cell&       operator()(size_t x, size_t y);
    const Cell& operator()(size_t x, size_t y) const;
    Cell&       operator()(const std::array<size_t, 2>& coord);
    const Cell& operator()(const std::array<size_t, 2>& coord) const;

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
signals:

    void memory_cell_changed(quint64 x, quint64 y, char value);

};


#endif //FUNGERA_MEMORY_H
