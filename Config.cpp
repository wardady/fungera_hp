// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "Config.h"

#include <fstream>
#include <sstream>

Config::Config() : memory_size{5000, 5000},
                   random_rate{5},
                   organism_death_rate{100},
                   max_num_of_mutations_rad{1},
                   stack_length{8},
                   cycle_gap{10000},
                   snapshot_rate{20000},
                   kill_if_no_child{25000},
                   memory_full_ratio{0.9},
                   kill_organisms_ratio{0.5},
                   radiation_mutation_rate{0.0},
                   mutation_on_reproduction_rate{0.0} {}

Config::Config(const std::string &path) {
    std::ifstream config_stream(path);
    if (!config_stream.is_open())
        throw std::invalid_argument("Failed to open configuration file");
    std::string temp;
    while (getline(config_stream, temp)) {
        temp.erase(std::remove_if(temp.begin(), temp.end(), isspace),
                   temp.end());
        if (temp[0] == '#' || temp.empty())
            continue;
        auto delimiterPos = temp.find('=');
        std::string name = temp.substr(0, delimiterPos);
        auto value = temp.substr(delimiterPos + 1);
        std::stringstream value_stream{value};
        std::transform(name.begin(), name.end(), name.begin(), ::tolower);
        if (name == "memory_size") {
            auto array_delimiter_pos = value.find(',');
            memory_size = {
                    std::stoull(value.substr(1, array_delimiter_pos - 1)),
                    std::stoull(value.substr(array_delimiter_pos + 1))};
        } else if (name == "random_rate") {
            value_stream >> random_rate;
        } else if (name == "organism_death_rate") {
            value_stream >> organism_death_rate;
        } else if (name == "max_num_of_mutations_rad") {
            value_stream >> max_num_of_mutations_rad;
        } else if (name == "stack_length") {
            value_stream >> stack_length;
        } else if (name == "cycle_gap") {
            value_stream >> cycle_gap;
        } else if (name == "snapshot_rate") {
            value_stream >> snapshot_rate;
        } else if (name == "kill_if_no_child") {
            value_stream >> kill_if_no_child;
        } else if (name == "memory_full_ratio") {
            value_stream >> memory_full_ratio;
        } else if (name == "kill_organisms_ratio") {
            value_stream >> kill_organisms_ratio;
        } else if (name == "radiation_mutation_rate") {
            value_stream >> radiation_mutation_rate;
        } else if (name == "mutation_on_reproduction_rate") {
            value_stream >> mutation_on_reproduction_rate;
        }
    }
}
