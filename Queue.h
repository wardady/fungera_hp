#ifndef FUNGERA_QUEUE_H
#define FUNGERA_QUEUE_H


#include <list>
#include <boost/serialization/access.hpp>
#include <boost/serialization/list.hpp>
#include "Organism.h"

class Organism;

class Queue {
    friend class boost::serialization::access;

    template<class Archive>
    void serialize(Archive &ar, unsigned int version) {
        ar & BOOST_SERIALIZATION_NVP(organisms);
        ar & BOOST_SERIALIZATION_NVP(kill_organisms_ratio);
    }

public:

    double kill_organisms_ratio;

    bool empty() {
        return organisms.empty();
    }

    Queue(const Queue &) = delete;

    void operator=(const Queue &) = delete;

    void kill_organisms();

    void push_organism(const Organism &rhs);

    void push_organism(Organism &&rhs);

    std::list<Organism>::iterator remove_organism(Organism &dead);

    explicit Queue(const double kill_organisms_ratio) : kill_organisms_ratio{
            kill_organisms_ratio} {}

    void cycle_all();

    size_t size();

    const std::list<Organism> &get_container() const;

    std::list<Organism> &_get_container();

private:
    std::list<Organism> organisms;

};

#endif //FUNGERA_QUEUE_H
