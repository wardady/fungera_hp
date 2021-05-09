// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include <functional>
#include <random>
#include <iostream>
#include "Organism.h"
#include "common.h"

size_t Organism::ID_seed;

bool Organism::operator<(const Organism &rhs) const {
    return errors_m < rhs.errors_m;
}

void Organism::nop() {
}

void Organism::move_up() {
    delta = {0, -1};
}

void Organism::move_down() {
    delta = {0, 1};
}

void Organism::move_right() {
    delta = {1, 0};
}

void Organism::move_left() {
    delta = {-1, 0};
}

char Organism::get_next_operand(size_t offset) {
    return (*memory_ptr_m)(get_shifted_ip(offset)).instruction;
}

void Organism::find_pattern() {
    std::string pattern;
    auto reg = get_next_operand(1);

    char instruction;
    for (size_t i{2}; i < std::max(size[0], size[1]); ++i) {
        if ((instruction = get_next_operand(i)) == '.' ||
            instruction == ':')
            pattern += (instruction == '.') ? ':' : '.';
        else
            break;
    }
    for (size_t i{0}, j{0}; i < std::max(size[0], size[1]); ++i) {
        if (get_next_operand(i) == pattern[j])
            ++j;
        else
            j = 0;
        if (j == pattern.size())
            registers.at(reg) = get_shifted_ip(i);
    }
}

std::array<size_t, 2> Organism::get_shifted_ip(size_t offset) {
    return {instruction_pointer[0] + offset * delta[0],
            instruction_pointer[1] + offset * delta[1]};
}

void Organism::if_not_zero() {
    auto instr = get_next_operand(1);
    if (instr == 'x' || instr == 'y') {
        // 3 if true 2 if false. TODO: if_not_zero?
        instruction_pointer = get_shifted_ip(2 +
                                             static_cast<bool>(registers.at(
                                                     get_next_operand(
                                                             2))[(instr == 'x')
                                                                 ? 0 : 1]));
    } else {
        instruction_pointer = get_shifted_ip(1 +
                                             (static_cast<bool>(registers.at(
                                                     instr)[0]) ||
                                              static_cast<bool>(registers.at(
                                                      instr)[1])));
    }
}

void Organism::inc() {
    auto instr = get_next_operand(1);
    if (instr == 'x' || instr == 'y')
        registers.at(get_next_operand(2))[(instr == 'x') ? 0 : 1]++;
    else {
        registers.at(instr)[0]++;
        registers.at(instr)[1]++;
    }
}

void Organism::dec() {
    auto instr = get_next_operand(1);
    if (instr == 'x' || instr == 'y')
        registers.at(get_next_operand(2))[(instr == 'x') ? 0 : 1]--;
    else {
        registers.at(instr)[0]--;
        registers.at(instr)[1]--;
    }
}

void Organism::zero() {
    auto instr = get_next_operand(1);
    registers.at(instr) = {0, 0};
}

void Organism::one() {
    auto instr = get_next_operand(1);
    registers.at(instr) = {1, 1};
}

void Organism::sub() {
    registers.at(get_next_operand(3)) = {
            registers.at(get_next_operand(1))[0] -
            registers.at((get_next_operand(2)))[0],
            registers.at(get_next_operand(1))[1] -
            registers.at((get_next_operand(2)))[1]};
}

void Organism::load_inst() {
    auto instruction = instructions.at(
            std::apply([this](auto x, auto y) { return (*memory_ptr_m)(x, y); },
                       registers.at(get_next_operand(1))).instruction).first;
    std::copy(instruction.begin(), instruction.end(),
              registers.at(get_next_operand(2)).begin());
}

void Organism::write_inst() {
    static std::random_device rd;
    static std::mt19937 gen(rd());

    static std::uniform_real_distribution<> prob_dist
            (0, std::nextafter(1, std::numeric_limits<double>::max()));

    if (!(child_size[0] == 0 && child_size[1] == 0)) {
        auto address = registers.at(get_next_operand(1));
        auto instruction_register = registers.at(get_next_operand(2));
        if (prob_dist(gen) < conf_ptr_m->mutation_on_reproduction_rate) {
            memory_ptr_m->set_cell_value(address[0], address[1], std::next(
                    Organism::instructions.begin(),
                    fungera::random(static_cast<size_t>(0),
                                    Organism::instructions.size()))->first);
        } else {
            for (const auto &inst:instructions) {
                if (inst.second.first[0] == instruction_register[0] &&
                    inst.second.first[1] == instruction_register[1]) {
                    memory_ptr_m->set_cell_value(address[0], address[1], inst.first);
                }
            }
        }
    }
}

void Organism::push() {
    if (stack.size() < conf_ptr_m->stack_length )
        stack.emplace_back(registers.at(get_next_operand(1)));
}

void Organism::pop() {
    if (stack.empty())
        throw std::out_of_range("Popping from empty stack!");
    registers.at(get_next_operand(1)) = stack.back();
    stack.pop_back();
}

void Organism::allocate_child() {
    auto requested_size = registers.at(get_next_operand(1));
    if (requested_size[0] <= 0 || requested_size[1] <= 0)
        throw std::invalid_argument("Requested child size must be > 0");
    for (size_t i{2}; i < std::max(conf_ptr_m->memory_size[0], conf_ptr_m->memory_size[1]); ++i) {
        auto is_allocated_region = memory_ptr_m->is_allocated_region(
                get_shifted_ip(i),
                requested_size);
        if (is_allocated_region == -1)
            throw std::out_of_range("Requested memory_ptr_m block is out of range!");
        if (is_allocated_region == 0) {
            child_entry_point = get_shifted_ip(i);
            registers.at(get_next_operand(2)) = child_entry_point;
            break;
        }
    }
    child_size = registers.at(get_next_operand(1));
    memory_ptr_m->allocate(child_entry_point, child_size);
}

void Organism::split_child() {
    if (child_size[0] != 0 && child_size[1] != 0) {
        number_of_children++;
        reproduction_cycle = 0;
        Organism child{child_size, child_entry_point, child_entry_point,
                       memory_ptr_m, organism_queue_ptr_m,
                       conf_ptr_m, id_m};
        children_id_list_m.emplace_back(child.get_id());
        organism_queue_ptr_m->push_organism(std::move(child));
        child_size = {0, 0};
        child_entry_point = {0, 0};
    }
}

Organism::Organism(std::array<std::size_t, 2> size,
                   std::array<std::size_t, 2> entry_point,
                   std::array<std::size_t, 2> begin, Memory *memory,
                   Queue *queue, Config *conf, size_t parent_id) :
        errors_m{}, instruction_pointer{entry_point},
        size{size}, memory_ptr_m{memory}, conf_ptr_m{conf}, organism_queue_ptr_m(queue),
        id_m{ID_seed++}, reproduction_cycle{}, number_of_children{0},
        child_size{}, child_entry_point{}, begin{begin}, children_id_list_m{},
        parent_id_m{parent_id}, commands_hm_m(size[1], size[0]) {
}

std::optional<std::list<Organism>::iterator> Organism::cycle() {
    try {
        if (instruction_pointer[0] < begin[0] ||
            instruction_pointer[1] < begin[1] ||
            instruction_pointer[0] > (begin[0] + size[0]) ||
            instruction_pointer[1] > (begin[1] + size[1])) {
            commands_hm_m(commands_hm_m.nrows, commands_hm_m.ncollumns)++;
        } else {
            std::array<size_t, 2> ip_position{};
            std::copy(instruction_pointer.begin(),
                      instruction_pointer.end(),
                      ip_position.begin());
            std::transform(ip_position.begin(), ip_position.end(),
                           ip_position.begin(),
                           [n = 0, this](size_t num)mutable {
                               return num - this->begin[n++];
                           });
            commands_hm_m(ip_position[1], ip_position[0])++;
        }
        (this->*instructions.at(get_next_operand(0)).second)();
    } catch (std::exception &e) {
        errors_m++;
    }
    instruction_pointer = get_shifted_ip(1);
    reproduction_cycle++;
    if (errors_m > conf_ptr_m->organism_death_rate ||
        reproduction_cycle > conf_ptr_m->kill_if_no_child) {
        return organism_queue_ptr_m->remove_organism(*this);
    }
    return {};
}

Organism::~Organism() {
    if (memory_ptr_m == nullptr)
        return;
    memory_ptr_m->deallocate(begin, size);
    if (child_size[0] != 0 || child_size[1] != 0)
        memory_ptr_m->deallocate(child_entry_point, child_size);
}

bool Organism::operator==(const Organism &rhs) const {
    return rhs.id_m == this->id_m;
}

//TODO: Set adequate moved-from object state.
Organism::Organism(Organism &&rhs)
noexcept: begin{rhs.begin}, errors_m{rhs.errors_m},
          instruction_pointer{rhs.instruction_pointer},
          size{rhs.size}, memory_ptr_m{rhs.memory_ptr_m}, conf_ptr_m{rhs.conf_ptr_m},
          organism_queue_ptr_m(rhs.organism_queue_ptr_m),
          id_m{rhs.id_m}, reproduction_cycle{rhs.reproduction_cycle},
          number_of_children{rhs.number_of_children},
          child_size{rhs.child_size}, child_entry_point{rhs.child_entry_point},
          stack{rhs.stack}, parent_id_m{rhs.parent_id_m}, children_id_list_m{rhs.children_id_list_m},
          commands_hm_m(rhs.commands_hm_m) {
    this->memory_ptr_m = rhs.memory_ptr_m;
    rhs.memory_ptr_m = nullptr;
}

const std::vector<size_t> &Organism::get_children() const {
    return children_id_list_m;
}

const size_t &Organism::get_parent() const {
    return parent_id_m;
}

const size_t &Organism::get_id() const {
    return id_m;
}


Organism &Organism::operator=(Organism &&rhs) noexcept {
    //TODO: Set adequate moved-from object state.
    begin = rhs.begin;
    errors_m = rhs.errors_m;
    instruction_pointer = rhs.instruction_pointer;
    size = rhs.size;
    memory_ptr_m = rhs.memory_ptr_m;
    conf_ptr_m = rhs.conf_ptr_m;
    organism_queue_ptr_m = rhs.organism_queue_ptr_m;
    id_m = rhs.id_m;
    reproduction_cycle = rhs.reproduction_cycle;
    number_of_children = rhs.number_of_children;
    child_size = rhs.child_size;
    child_entry_point = rhs.child_entry_point;
    stack = rhs.stack;
    children_id_list_m = rhs.children_id_list_m;
    parent_id_m = rhs.parent_id_m;
    commands_hm_m = rhs.commands_hm_m;


    this->memory_ptr_m = rhs.memory_ptr_m;
    rhs.memory_ptr_m = nullptr;

    return *this;
}

Organism::Organism() : commands_hm_m{0, 0} {

}

size_t Organism::get_errors() const {
    return errors_m;
}

const std::array<size_t, 2>& Organism::get_ip() const {
    return instruction_pointer;
}

const std::array<int8_t, 2>& Organism::get_delta() const {
    return delta;
}

const std::unordered_map<char, std::array<std::size_t, 2>> &
Organism::get_registers() const {
    return registers;
}

const std::vector<std::array<size_t, 2>> &Organism::get_stack() const {
    return stack;
}

size_t Organism::get_id_seed() {
    return ID_seed;
}

const std::array<size_t, 2> &Organism::get_start() const {
    return begin;
}

const std::array<size_t, 2> &Organism::get_size() const  {
    return size;
}

bool Organism::is_ip_within() const{
    const auto beg_row = get_start()[1];
    const auto fin_row = get_start()[1] + get_size()[1];
    const auto beg_col = get_start()[0];
    const auto fin_col = get_start()[0] + get_size()[0];
    return instruction_pointer[1] >= beg_row && instruction_pointer[1] < fin_row &&
           instruction_pointer[0] >= beg_col && instruction_pointer[0] < fin_col;
}

bool Organism::is_ip_on_border() const{
    const auto beg_row = get_start()[1];
    const auto fin_row = get_start()[1] + get_size()[1];
    const auto beg_col = get_start()[0];
    const auto fin_col = get_start()[0] + get_size()[0];

    return is_ip_within() && (
            instruction_pointer[1] == beg_row || instruction_pointer[1] == fin_row-1 ||
            instruction_pointer[0] == beg_col || instruction_pointer[0] == fin_col-1
                             );
}

const std::unordered_map<char, std::pair<std::array<uint8_t, 2>, Organism::instruction>>
Organism::instructions{
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

