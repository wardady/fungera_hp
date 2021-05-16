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
#include <random>
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
        ar & BOOST_SERIALIZATION_NVP(total_organism_num);
        ar & BOOST_SERIALIZATION_NVP(instruction_pointer);
        ar & BOOST_SERIALIZATION_NVP(size);
        ar & BOOST_SERIALIZATION_NVP(child_entry_point);
        ar & BOOST_SERIALIZATION_NVP(child_size);
        ar & BOOST_SERIALIZATION_NVP(begin);
        ar & BOOST_SERIALIZATION_NVP(delta);
        ar & BOOST_SERIALIZATION_NVP(*static_cast<gp_reg_t (*)[registers_N]>(
                                         static_cast<void *>(registers.data())));
            // Boost так робить для std::array.
        ar & BOOST_SERIALIZATION_NVP(stack);
        ar & BOOST_SERIALIZATION_NVP(children_id_list_m);
        ar & BOOST_SERIALIZATION_NVP(id_m);
        ar & BOOST_SERIALIZATION_NVP(parent_id_m);
        ar & BOOST_SERIALIZATION_NVP(commands_hm_m);
    }

    Organism();

public:
    using general_purpose_reg_base_t  = size_t; //! TODO: use signed type
    using memory_index_base_t = size_t;
    using gp_reg_t = std::array<general_purpose_reg_base_t, 2>;
    using mem_reg_t = std::array<memory_index_base_t, 2>;
    using delta_reg_t = std::array<int, 2>;
    using stack_t = std::vector<std::array<size_t, 2>>;

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

    auto get_errors() const { return errors_m; };

    const auto& get_ip() const { return instruction_pointer; };

    const auto& get_delta() const { return delta; };

    const auto& get_registers() const { return registers; } // Тут -- заради auto спереду.

    const auto& get_start() const { return begin; };

    const auto& get_size() const { return size; };

    const auto& get_stack() const { return stack; };

    static auto get_total_organism_num() { return total_organism_num; };

    using instruction = void (Organism::*)();
    static const std::unordered_map<char, std::pair<std::array<uint8_t, 2>, instruction>> instructions;

    const auto& get_children_id_list() const { return children_id_list_m; };

    auto get_parent() const { return parent_id_m; };

    auto get_id() const { return id_m; }

    bool is_ip_within() const;
    bool is_ip_on_border() const;

    static bool is_correct_opcode(general_purpose_reg_base_t opcode);
    bool get_random_opcode_base() const;

    bool operator<(const Organism &rhs) const;

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

    mem_reg_t get_shifted_ip(size_t offset);

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

    size_t reproduction_cycle = 0;
    size_t number_of_children = 0;
    static size_t total_organism_num;

    mem_reg_t instruction_pointer{0, 0};
    mem_reg_t size{0, 0};
    mem_reg_t child_entry_point{0, 0};
    mem_reg_t child_size{0, 0};
    mem_reg_t begin{0, 0};
    delta_reg_t delta{1, 0};

    static constexpr size_t registers_N = 4;
    struct registers_t: private std::array<gp_reg_t, registers_N> { // Тут вже я перекреативив трохи, але поки хай буде...
        using array::size;
        using array::begin;
        using array::cbegin;
        using array::end;
        using array::cend;
        using array::data;

        registers_t() = default;
        registers_t(const gp_reg_t& init);

        gp_reg_t& at_index(size_t idx){
            return static_cast<std::array<gp_reg_t, registers_N>*>(this)->at(idx);
        }
        const gp_reg_t& at_index(size_t idx) const {
            return static_cast<const std::array<gp_reg_t, registers_N>*>(this)->at(idx);
        }

        gp_reg_t& at_opcode(char opcode){
            return static_cast<std::array<gp_reg_t, registers_N>*>(this)->at(operand_to_register(opcode));
        }
        const gp_reg_t& at_opcode(char opcode) const{
            return static_cast<const std::array<gp_reg_t, registers_N>*>(this)->at(operand_to_register(opcode));
        }
    private:
        static size_t operand_to_register(char op);
    };

    registers_t  registers{ gp_reg_t{0, 0} };
    static size_t operand_to_scalar(char op);

    stack_t stack;

    std::vector<size_t> children_id_list_m;
    size_t id_m = 0;
    size_t parent_id_m = 0;
};

#endif //FUNGERA_ORGANISM_H
