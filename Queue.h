#ifndef FUNGERA_QUEUE_H
#define FUNGERA_QUEUE_H

#include <vector>
#include "Organism.h"

class Queue {
public:
    Queue(const Queue &) = delete;

    void operator=(const Queue &) = delete;

    void kill_organisms();

    Queue(const double kill_organisms_ratio) : kill_organisms_ratio{kill_organisms_ratio} {}

private:
    std::vector<Organism> organisms;
    const double kill_organisms_ratio;
};

#endif //FUNGERA_QUEUE_H
