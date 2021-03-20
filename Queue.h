#ifndef FUNGERA_QUEUE_H
#define FUNGERA_QUEUE_H


#include <vector>
#include "Organism.h"

class Organism;

class Queue {
public:
    Queue(const Queue &) = delete;

    void operator=(const Queue &) = delete;

    void kill_organisms();

    void emplace_organism(Organism &rhs);

    void emplace_organism(Organism &&rhs);

    explicit Queue(const double kill_organisms_ratio) : kill_organisms_ratio{
            kill_organisms_ratio} {}

    void cycle_all();

private:
    std::vector<Organism> organisms;
    const double kill_organisms_ratio;

};

#endif //FUNGERA_QUEUE_H
