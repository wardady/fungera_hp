#ifndef FUNGERA_CONFIG_H
#define FUNGERA_CONFIG_H

#include <string>
#include <array>
#include <boost/serialization/access.hpp>
#include <boost/serialization/array.hpp>

class Config {
    friend class boost::serialization::access;

    template<class Archive>
    void serialize(Archive &ar, unsigned int version) {
        ar & BOOST_SERIALIZATION_NVP(memory_size);
        ar & BOOST_SERIALIZATION_NVP(random_rate);
        ar & BOOST_SERIALIZATION_NVP(organism_death_rate);
        ar & BOOST_SERIALIZATION_NVP(max_num_of_mutations_rad);
        ar & BOOST_SERIALIZATION_NVP(cycle_gap);
        ar & BOOST_SERIALIZATION_NVP(stack_length);
        ar & BOOST_SERIALIZATION_NVP(kill_if_no_child);
        ar & BOOST_SERIALIZATION_NVP(memory_full_ratio);
        ar & BOOST_SERIALIZATION_NVP(kill_organisms_ratio);
        ar & BOOST_SERIALIZATION_NVP(radiation_mutation_rate);
        ar & BOOST_SERIALIZATION_NVP(mutation_on_reproduction_rate);
        ar & BOOST_SERIALIZATION_NVP(snapshot_rate);
    }

public:
    // TODO: implement config parser
    std::array<size_t, 2> memory_size{5000, 5000};
    int random_rate = 5, organism_death_rate = 100, max_num_of_mutations_rad = 1, stack_length = 8;
    size_t cycle_gap = 10000, snapshot_rate = 20000, kill_if_no_child = 25000;
    double memory_full_ratio = 0.9, kill_organisms_ratio = 0.5, radiation_mutation_rate = 0.0, mutation_on_reproduction_rate = 0.0;

    explicit Config(const std::string &path) {};

};


#endif //FUNGERA_CONFIG_H
