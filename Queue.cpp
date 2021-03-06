// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "Queue.h"
#include <algorithm>

void Queue::kill_organisms() {
    std::sort(organisms.rbegin(), organisms.rend());
    size_t ratio = static_cast<size_t>(organisms.size() * kill_organisms_ratio);
    organisms.erase(organisms.begin(), organisms.begin() + ratio);
}
