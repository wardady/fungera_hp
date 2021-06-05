// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include <functional>
#include <iterator>
#include <random>
#include <iostream>
#include <algorithm>
#include "Organism.h"
#include "common.h"

size_t Organism::total_organism_num;

bool Organism::operator<(const Organism &rhs) const {
    return errors_m < rhs.errors_m;
}

size_t Organism::registers_t::operand_to_register(char op) {
    int reg_idx = op - 'a';
    if (reg_idx >= Organism::registers_N)
        return static_cast<size_t>(-1);
    else
        return reg_idx;
}

Organism::registers_t::registers_t(const gp_reg_t &init) {
    std::fill(begin(), end(), init);
}

size_t Organism::operand_to_scalar(char op) {
    if (op == 'x')
        return 1;
    else if (op == 'y')
        return 0;
    else
        return static_cast<size_t>(-1);
}

std::array<size_t, 2> Organism::get_shifted_ip(size_t offset) {
    return {instruction_pointer[0] + offset * delta[0],
            instruction_pointer[1] + offset * delta[1]};
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
    // Перевірку, чи символ є патерном, варто перенести в окрему функцію,
    // як і знаходження компланарного елемента патерна.
    //! Питання: чому пошук лише в межах розміру?
    std::string pattern;
    auto reg = get_next_operand(1);

    char instruction;
    for (size_t i = 2; i < std::max(size[0], size[1]); ++i) {
        instruction = get_next_operand(i);
        if (instruction == '.' || instruction == ':')
            pattern += (instruction == '.') ? ':' : '.';
        else
            break;
    }
    for (size_t i = 0, j = 0; i < std::max(size[0], size[1]); ++i) {
        if (get_next_operand(i) == pattern[j])
            ++j;
        else
            j = 0;
        if (j == pattern.size()) {
            registers.at_opcode(reg) = get_shifted_ip(i);
            break;
        }
    }
}

void Organism::if_not_zero() {
    auto instr = get_next_operand(1);
    if (instr == 'x' || instr == 'y') {
        // 3 if true 2 if false. TODO: if_not_zero?
        auto reg = get_next_operand(2);
        instruction_pointer = get_shifted_ip(
                2 + static_cast<bool>(
                        registers.at_opcode(reg)[operand_to_scalar(instr)] !=
                        0 ));
    } else {
        auto reg = get_next_operand(1);
        instruction_pointer = get_shifted_ip(
                1 + static_cast<bool>( registers.at_opcode(reg) !=
                                       gp_reg_t{0, 0} ));
    }
}

void Organism::inc() {
    auto instr = get_next_operand(1);
    if (instr == 'x' || instr == 'y') {
        auto reg = get_next_operand(2);
        ++registers.at_opcode(reg)[operand_to_scalar(instr)];
    } else {
        ++registers.at_opcode(instr)[0];
        ++registers.at_opcode(instr)[1];
    }
}

void Organism::dec() {
    auto instr = get_next_operand(1);
    if (instr == 'x' || instr == 'y') {
        auto reg = get_next_operand(2);
        --registers.at_opcode(reg)[operand_to_scalar(instr)];
    } else {
        --registers.at_opcode(instr)[0];
        --registers.at_opcode(instr)[1];
    }
}

void Organism::zero() {
    auto reg = get_next_operand(1);
    registers.at_opcode(reg) = {0, 0};
}

void Organism::one() {
    auto reg = get_next_operand(1);
    registers.at_opcode(reg) = {1, 1};
}

void Organism::sub() {
    registers.at_opcode(get_next_operand(3)) = {
            registers.at_opcode(get_next_operand(1))[0] -
            registers.at_opcode((get_next_operand(2)))[0],
            registers.at_opcode(get_next_operand(1))[1] -
            registers.at_opcode((get_next_operand(2)))[1]};
}

void Organism::load_inst() {
    auto instruction = (*memory_ptr_m)(
            registers.at_opcode(get_next_operand(1))).instruction;
    registers.at_opcode(get_next_operand(2)) = gp_reg_t{0,
                                                        static_cast<general_purpose_reg_base_t>(instruction)};
}

void Organism::write_inst() {
    auto instruction_r = registers.at_opcode(get_next_operand(2))[1];
    if (!Organism::is_correct_opcode(instruction_r)) {
        ++errors_m; // Взагалі, хоча фокус із at() зручний, але дуже вже повільний -- потрібно буде всюди переробити.
        return;
    }
    char instruction = static_cast<char>(instruction_r);
    auto address = registers.at_opcode(get_next_operand(1));
    if (conf_ptr_m->has_mutated_on_copy()) {
        instruction = get_random_opcode_base(conf_ptr_m);
    }
    memory_ptr_m->set_cell_value(address[0], address[1], instruction);

}

void Organism::push() {
    if (stack.size() < conf_ptr_m->stack_length) {
        stack.emplace_back(registers.at_opcode(get_next_operand(1)));
    } else {
        ++errors_m;
    }
}

void Organism::pop() {
    if (stack.empty())
        throw std::out_of_range("Popping from empty stack!");
    registers.at_opcode(get_next_operand(1)) = stack.back();
    stack.pop_back();
}

void Organism::allocate_child() {
    auto requested_size = registers.at_opcode(get_next_operand(1));
    if (requested_size[0] <= 0 || requested_size[1] <= 0)
        throw std::invalid_argument("Requested child size must be > 0");
    for (size_t i = 2; i < std::max(conf_ptr_m->memory_size[0],
                                    conf_ptr_m->memory_size[1]); ++i) {
        //! TODO: Цю частину треба таки переробити! Вона жахливо повільна, і існують кращі алгоритми.
        //! Ймовірно -- підтримувати bitmap зайнятих комірок і шукати по ньому.
        //! Крім того, не варто починати шукати вільне ВСЕРЕДИНІ організму.
        auto is_allocated_region = memory_ptr_m->is_allocated_region(
                get_shifted_ip(i),
                requested_size);
        if (is_allocated_region == -1)
            throw std::out_of_range(
                    "Requested memory_ptr_m block is out of range!");
        if (is_allocated_region == 0) {
            child_entry_point = get_shifted_ip(i);
            registers.at_opcode(get_next_operand(2)) = child_entry_point;
            break;
        }
    }
    child_size = registers.at_opcode(get_next_operand(1));
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

Organism::Organism(std::array<std::size_t, 2> size_,
                   std::array<std::size_t, 2> entry_point,
                   std::array<std::size_t, 2> begin_, Memory *memory,
                   Queue *queue, Config *conf, size_t parent_id) :
        memory_ptr_m{memory}, organism_queue_ptr_m(queue), conf_ptr_m{conf},
        errors_m{}, commands_hm_m(size_[1], size_[0]),
        reproduction_cycle{0}, number_of_children{0},
        instruction_pointer{entry_point}, size{size_},
        child_entry_point{}, child_size{}, begin{begin_}, children_id_list_m{},
        id_m{total_organism_num++}, parent_id_m{parent_id} {
}

std::optional<std::list<Organism>::iterator> Organism::cycle() {
    try {
        if (instruction_pointer[0] < begin[0] ||
            instruction_pointer[1] < begin[1] ||
            instruction_pointer[0] > (begin[0] + size[0]) ||
            instruction_pointer[1] > (begin[1] + size[1])) {
            commands_hm_m(commands_hm_m.nrows, commands_hm_m.ncollumns)++;
        } else {
            ++commands_hm_m(instruction_pointer[1] - begin[1],
                            instruction_pointer[0] - begin[0]);
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
Organism::Organism(Organism &&rhs) noexcept:
        begin{rhs.begin}, errors_m{rhs.errors_m},
        instruction_pointer{rhs.instruction_pointer},
        size{rhs.size}, memory_ptr_m{rhs.memory_ptr_m},
        conf_ptr_m{rhs.conf_ptr_m},
        organism_queue_ptr_m(rhs.organism_queue_ptr_m),
        id_m{rhs.id_m}, reproduction_cycle{rhs.reproduction_cycle},
        number_of_children{rhs.number_of_children},
        child_size{rhs.child_size}, child_entry_point{rhs.child_entry_point},
        stack{rhs.stack}, parent_id_m{rhs.parent_id_m},
        children_id_list_m{rhs.children_id_list_m},
        commands_hm_m(rhs.commands_hm_m) {
    this->memory_ptr_m = rhs.memory_ptr_m;
    rhs.memory_ptr_m = nullptr;
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

bool Organism::is_ip_within() const {
    const auto beg_row = get_start()[1];
    const auto fin_row = get_start()[1] + get_size()[1];
    const auto beg_col = get_start()[0];
    const auto fin_col = get_start()[0] + get_size()[0];
    return instruction_pointer[1] >= beg_row &&
           instruction_pointer[1] < fin_row &&
           instruction_pointer[0] >= beg_col &&
           instruction_pointer[0] < fin_col;
}

bool Organism::is_ip_on_border() const {
    const auto beg_row = get_start()[1];
    const auto fin_row = get_start()[1] + get_size()[1];
    const auto beg_col = get_start()[0];
    const auto fin_col = get_start()[0] + get_size()[0];

    return is_ip_within() && (
            instruction_pointer[1] == beg_row ||
            instruction_pointer[1] == fin_row - 1 ||
            instruction_pointer[0] == beg_col ||
            instruction_pointer[0] == fin_col - 1
    );
}

bool Organism::is_correct_opcode(general_purpose_reg_base_t opcode) {
    if (opcode > 255 ||
        opcode < 0) { // <= important for signed types -- silence warnings.
        return false;
    }
    char opcode_c = static_cast<char>(opcode);
    return Organism::instructions.find(opcode_c) !=
           Organism::instructions.end();
}

//! Base -- тому що потім будуть інші, наприклад, обмеження що в що мутує чи нерівномірний розподіл.
char Organism::get_random_opcode_base(Config* conf) {
    auto next_idx = conf->random<size_t>(0,
                                               Organism::instructions.size() -
                                               1);
    auto beg = instructions.cbegin();
    std::advance(beg, next_idx);
    return beg->first;
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

bool Organism::operator==(const size_t id) const{
    return id_m == id;
}

