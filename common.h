#ifndef FUNGERA_COMMON_H
#define FUNGERA_COMMON_H
#include <vector>
#include <random>
namespace fungera {

    template<typename T>
    T random(T from, T to) {
        // Generates random number from range [from,to)
        static std::random_device rd;
        static std::mt19937 gen{rd()};
        std::vector<int> weights(to - from, 1);
        std::discrete_distribution<T> dist(weights.begin(), weights.end());
        return from + dist(gen);
    }

}
#endif //FUNGERA_COMMON_H
