#ifndef FUNGERA_QUEUE_H
#define FUNGERA_QUEUE_H


#include <list>
#include "Organism.h"

class Organism;

class Queue {
public:

    bool empty(){
        return organisms.empty();
    }

    Queue(const Queue &) = delete;

    void operator=(const Queue &) = delete;

    void kill_organisms();

    void push_organism(const Organism &rhs);

    void push_organism(Organism &&rhs);

    std::_List_iterator<Organism> remove_organism(Organism &dead);

    explicit Queue(const double kill_organisms_ratio) : kill_organisms_ratio{
            kill_organisms_ratio} {}

    void cycle_all();

private:
    std::list<Organism> organisms;
    const double kill_organisms_ratio;

};

#endif //FUNGERA_QUEUE_H
