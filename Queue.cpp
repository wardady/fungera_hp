#include "Queue.h"

void Queue::kill_organisms() {
    std::sort(organisms.rbegin(), organisms.rend());
    size_t ratio = organisms.size() * kill_organisms_ratio;
    organisms.erase(organisms.begin(), organisms.begin() + ratio);
}
