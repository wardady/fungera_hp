#ifndef FUNGERA_ORGANISM_H
#define FUNGERA_ORGANISM_H


#include <cstddef>
#include <unordered_map>
#include <boost/serialization/access.hpp>
#include <boost/serialization/unordered_map.hpp>
#include <boost/serialization/deque.hpp>
#include <boost/serialization/stack.hpp>
#include <cstdint>
#include <optional>
#include "Memory.h"
#include "Config.h"
#include "Queue.h"
#include "CommandHeatMap.h"

class Queue;

class Organism {
    friend class boost::serialization::access;

    template<class Archive>
    void serialize(Archive &ar, unsigned int version) {
        ar & BOOST_SERIALIZATION_NVP(errors_m);
        ar & BOOST_SERIALIZATION_NVP(reproduction_cycle);
        ar & BOOST_SERIALIZATION_NVP(number_of_children);
        ar & BOOST_SERIALIZATION_NVP(ID_seed);
        ar & BOOST_SERIALIZATION_NVP(instruction_pointer);
        ar & BOOST_SERIALIZATION_NVP(size);
        ar & BOOST_SERIALIZATION_NVP(child_entry_point);
        ar & BOOST_SERIALIZATION_NVP(child_size);
        ar & BOOST_SERIALIZATION_NVP(begin);
        ar & BOOST_SERIALIZATION_NVP(delta);
        ar & BOOST_SERIALIZATION_NVP(registers);
        ar & BOOST_SERIALIZATION_NVP(stack);
        ar & BOOST_SERIALIZATION_NVP(children_id_list_m);
        ar & BOOST_SERIALIZATION_NVP(id_m);
        ar & BOOST_SERIALIZATION_NVP(parent_id_m);
        ar & BOOST_SERIALIZATION_NVP(commands_hm_m);
    }

    Organism();

public:
    bool operator<(const Organism &rhs) const;

    Organism(std::array<std::size_t, 2> size,
             std::array<std::size_t, 2> entry_point,
             std::array<std::size_t, 2> begin, Memory *memory,
             Queue *queue, Config *conf, size_t parent_id = 0);

    std::optional<std::list<Organism>::iterator> cycle();

    bool operator==(const Organism &rhs) const;

    Organism& operator=(const Organism &rhs) = default;

    Organism(const Organism &rhs) = default;

    Organism(Organism &&rhs) noexcept;

    Organism &operator=(Organism &&rhs) noexcept;

    ~Organism();

    size_t get_errors() const;

    const std::array<size_t, 2>& get_ip() const;

    const std::array<int8_t, 2>& get_delta() const;

    const std::unordered_map<char, std::array<long long, 2>> &
    get_registers() const;

    const std::array<size_t,2> & get_start() const;

    const std::array<size_t,2> & get_size() const;

    const std::vector<std::array<long long, 2>> &get_stack() const;

    static size_t get_total_organism_num();

    using instruction = void (Organism::*)();
    static const std::unordered_map<char, std::pair<std::array<uint8_t, 2>, instruction>> instructions;

    const std::vector<size_t>& get_children() const;

    const size_t& get_parent() const;

    const size_t& get_id() const;

    bool is_ip_within() const;
    bool is_ip_on_border() const;

// private:
    Memory *memory_ptr_m = nullptr;
    Queue *organism_queue_ptr_m  = nullptr;
    Config *conf_ptr_m  = nullptr;

    size_t errors_m = 0; // TODO: Move to private (here just for debug)
    CommandHeatMap commands_hm_m;

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

    size_t reproduction_cycle = 0, number_of_children = 0;
    static size_t ID_seed;
    std::array<size_t, 2> instruction_pointer{0, 0};
    std::array<size_t, 2> size{0, 0};
    std::array<size_t, 2> child_entry_point{0, 0};
    std::array<size_t, 2> child_size{0, 0};
    std::array<size_t, 2> begin{0, 0};
    std::array<int8_t, 2> delta{1, 0};
    std::unordered_map<char, std::array<long long, 2>> registers{
            {'a', {0, 0}},
            {'b', {0, 0}},
            {'c', {0, 0}},
            {'d', {0, 0}}
    };
    std::vector<std::array<long long, 2>> stack;

    std::vector<size_t> children_id_list_m;
    size_t id_m = 0;
    size_t parent_id_m = 0;
};

#endif //FUNGERA_ORGANISM_H
