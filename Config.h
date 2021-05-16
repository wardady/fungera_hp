#ifndef FUNGERA_CONFIG_H
#define FUNGERA_CONFIG_H

#include <string>
#include <array>
#include <random>
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
    size_t cycle_gap, snapshot_rate, kill_if_no_child, random_rate, organism_death_rate, max_num_of_mutations_rad, stack_length;
    double memory_full_ratio, kill_organisms_ratio, radiation_mutation_rate, mutation_on_reproduction_rate;

    explicit Config(const std::string &path);

    Config();

    //! TODO: Save and restore seed!
    std::random_device rd;
    mutable std::mt19937 gen{rd()};
    mutable std::uniform_real_distribution<double> uni_d_0_1{0, 1}; // {0, std::nextafter(1, 2)}?

    bool has_mutated_on_copy() const {
        if( mutation_on_reproduction_rate == 0.0 )
            return false; // Оптимізація -- рандомний генератор повільний. Тут порівнювати fp == 0.0 -- коректно.
        return uni_d_0_1(gen) < mutation_on_reproduction_rate;
    }

    template<typename T>
    T random(T from, T to) {
        // Generates random number from range [from,to)
        static std::uniform_int_distribution<T> ud(from, to);
        return ud(gen);
    }


};


#endif //FUNGERA_CONFIG_H
