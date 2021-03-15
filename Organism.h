#ifndef FUNGERA_ORGANISM_H
#define FUNGERA_ORGANISM_H


#include <cstddef>
#include <unordered_map>
#include <cstdint>
#include <stack>
#include "Memory.h"
#include "Config.h"
#include "Queue.h"

class Queue;

class Organism {
public:
    bool operator<(const Organism &rhs) const;

    Organism(std::array<std::size_t, 2> &size,
             std::array<std::size_t, 2> &entry_point, Memory *memory,
             Queue *queue, Config *conf);

private:
    void nop();

    void move_up();

    void move_down();

    void move_right();

    void move_left();

    std::array<size_t, 2> get_shifted_ip(size_t offset);

    char get_next_operand(size_t offset);

    void find_pattern();

    void if_not_zero();

    void one();

    void zero();

    void dec();

    void inc();

    void sub();

    void load_inst();

    void write_inst();

    void allocate_child();

    void split_child();

    void push();

    void pop();


    size_t errors, id, reproduction_cycle, number_of_children;
    static size_t ID_seed;
    std::array<size_t, 2> instruction_pointer, size,
            child_entry_point, child_size;
    std::array<int8_t, 2> delta{1, 0};
    std::unordered_map<char, std::array<std::size_t, 2>> registers{
            {'a', {0, 0}},
            {'b', {0, 0}},
            {'c', {0, 0}},
            {'d', {0, 0}}
    };
    Memory *memory;
    std::stack<std::array<size_t, 2>> stack;

    using instruction = void (Organism::*)();
    static const inline std::unordered_map<char,
            std::pair<std::array<uint8_t, 2>, instruction>> instructions{
            {'.', {{0, 0}, &Organism::nop}},
            {':', {{0, 1}, &Organism::nop}},
            {'a', {{1, 0}, &Organism::nop}},
            {'b', {{1, 1}, &Organism::nop}},
            {'c', {{1, 2}, &Organism::nop}},
            {'d', {{1, 3}, &Organism::nop}},
            {'x', {{2, 0}, &Organism::nop}},
            {'y', {{2, 1}, &Organism::nop}},
            {'^', {{3, 0}, &Organism::move_up}},
            {'v', {{3, 1}, &Organism::move_down}},
            {'>', {{3, 2}, &Organism::move_right}},
            {'<', {{3, 3}, &Organism::move_left}},
            {'&', {{4, 0}, &Organism::find_pattern}},
            {'?', {{5, 0}, &Organism::if_not_zero}},
            {'1', {{6, 0}, &Organism::one}},
            {'0', {{6, 1}, &Organism::zero}},
            {'-', {{6, 2}, &Organism::dec}},
            {'+', {{6, 3}, &Organism::inc}},
            {'~', {{6, 4}, &Organism::sub}},
            {'L', {{7, 0}, &Organism::load_inst}},
            {'W', {{7, 1}, &Organism::write_inst}},
            {'@', {{7, 2}, &Organism::allocate_child}},
            {'$', {{7, 3}, &Organism::split_child}},
            {'S', {{8, 0}, &Organism::push}},
            {'P', {{8, 1}, &Organism::pop}}
    };
    Queue *organism_queue;
    Config *c;
};


#endif //FUNGERA_ORGANISM_H
