// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include <functional>
#include <random>
#include "Organism.h"
#include "common.h"

size_t Organism::ID_seed;

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
    delta = {1, 0};
}

void Organism::move_left() {
    delta = {-1, 0};
}

char Organism::get_next_operand(size_t offset) {
    return std::apply([this](auto x, auto y) { return (*memory)(x, y); },
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
            std::apply([this](auto x, auto y) { return (*memory)(x, y); },
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
        auto &cell = (*memory)(address[0], address[1]);
        auto instruction_register = registers.at(get_next_operand(2));
        if (prob_dist(gen) < c->mutation_on_reproduction_rate) {
            cell.instruction = std::next(
                    Organism::instructions.begin(),
                    fungera::random(static_cast<size_t>(0),
                                    Organism::instructions.size()))->first;
        } else {
            for (const auto &inst:instructions) {
                if (inst.second.first[0] == instruction_register[0] &&
                    inst.second.first[1] == instruction_register[1]) {
                    cell.instruction = inst.first;
                }
            }
        }
    }
}

void Organism::push() {
    if (stack.size() < c->stack_length)
        stack.emplace(registers.at(get_next_operand(1)));
}

void Organism::pop() {
    if (stack.empty())
        throw std::out_of_range("Popping from empty stack!");
    registers.at(get_next_operand(1)) = stack.top();
    stack.pop();
}

void Organism::allocate_child() {
    auto requested_size = registers.at(get_next_operand(1));
    if (requested_size[0] <= 0 || requested_size[1] <= 0) {
        // TODO: not an error?
        return;
    }
    for (size_t i{0}; i < std::max(c->memory_size[0], c->memory_size[1]); ++i) {
        auto is_allocated_region = memory->is_allocated_region(
                get_shifted_ip(i),
                requested_size);
        if (is_allocated_region == -1)
            throw std::out_of_range("Requested memory block is out of range!");
        if (is_allocated_region == 0) {
            child_entry_point = get_shifted_ip(i);
            registers.at(get_next_operand(2)) = child_entry_point;
            break;
        }
    }
    child_size = registers.at(get_next_operand(1));
    memory->allocate(child_entry_point, child_size);
}

void Organism::split_child() {
    if (child_size[0] != 0 && child_size[1] != 0) {
        number_of_children++;
        reproduction_cycle = 0;
        organism_queue->push_organism(
                Organism{child_size, child_entry_point, child_entry_point,
                         memory, organism_queue,
                         c});
        child_size = {0, 0};
        child_entry_point = {0, 0};
    }
}

Organism::Organism(std::array<std::size_t, 2> size,
                   std::array<std::size_t, 2> entry_point,
                   std::array<std::size_t, 2> begin, Memory *memory,
                   Queue *queue, Config *conf) :
        errors{}, instruction_pointer{entry_point},
        size{size}, memory{memory}, c{conf}, organism_queue(queue),
        id{ID_seed++}, reproduction_cycle{}, number_of_children{0},
        child_size{}, child_entry_point{}, begin{begin} {
}

std::optional<std::list<Organism>::iterator> Organism::cycle() {
    try {
        (this->*instructions.at(get_next_operand(0)).second)();
    } catch (std::exception &e) {
        errors++;
    }
    instruction_pointer = get_shifted_ip(1);
    reproduction_cycle++;
    if (errors > c->organism_death_rate ||
        reproduction_cycle > c->kill_if_no_child) {
        return organism_queue->remove_organism(*this);
    }
    return {};
}

Organism::~Organism() {
    if (memory == nullptr)
        return;
    memory->deallocate(begin, size);
    if (child_size[0] != 0 || child_size[1] != 0)
        memory->deallocate(child_entry_point, child_size);
}

bool Organism::operator==(const Organism &rhs) const {
    return rhs.id == this->id;
}

//TODO: Set adequate moved-from object state.
Organism::Organism(Organism &&rhs)
noexcept: begin{rhs.begin}, errors{rhs.errors},
          instruction_pointer{rhs.instruction_pointer},
          size{rhs.size}, memory{rhs.memory}, c{rhs.c},
          organism_queue(rhs.organism_queue),
          id{rhs.id}, reproduction_cycle{rhs.reproduction_cycle},
          number_of_children{rhs.number_of_children},
          child_size{rhs.child_size}, child_entry_point{rhs.child_entry_point},
          stack{rhs.stack} {
    this->memory = rhs.memory;
    rhs.memory = nullptr;
}

Organism &Organism::operator=(Organism &&rhs) noexcept {
    //TODO: Set adequate moved-from object state.
    begin = rhs.begin;
    errors = rhs.errors;
    instruction_pointer = rhs.instruction_pointer;
    size = rhs.size;
    memory = rhs.memory;
    c = rhs.c;
    organism_queue = rhs.organism_queue;
    id = rhs.id;
    reproduction_cycle = rhs.reproduction_cycle;
    number_of_children = rhs.number_of_children;
    child_size = rhs.child_size;
    child_entry_point = rhs.child_entry_point;
    stack = rhs.stack;

    this->memory = rhs.memory;
    rhs.memory = nullptr;

    return *this;
}
