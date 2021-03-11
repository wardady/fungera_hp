// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include <functional>
#include "Organism.h"


bool Organism::operator<(const Organism &rhs) const {
    return errors < rhs.errors;
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
    delta = {0, 1};
}

void Organism::move_left() {
    delta = {0, -1};
}

char Organism::get_next_operand(size_t offset) {
    return std::apply([this](auto x, auto y) { return memory(x, y); },
                      get_shifted_ip(offset)).instruction;
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
        // 3 if true 2 if false. TODO: if_not_thero?
        instruction_pointer = get_shifted_ip(2 +
                                             static_cast<bool>(registers[get_next_operand(
                                                     2)][(instr == 'x')
                                                         ? 0 : 1]));
    } else {
        instruction_pointer = get_shifted_ip(1 +
                                             (static_cast<bool>(registers[instr][0]) ||
                                              static_cast<bool>(registers[instr][1])));
    }
}

