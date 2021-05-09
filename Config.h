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
    std::array<size_t, 2> memory_size;
    int random_rate, organism_death_rate, max_num_of_mutations_rad, stack_length;
    size_t cycle_gap, snapshot_rate, kill_if_no_child;
    double memory_full_ratio, kill_organisms_ratio, radiation_mutation_rate, mutation_on_reproduction_rate;

    explicit Config(const std::string &path);

    Config();

};


#endif //FUNGERA_CONFIG_H
