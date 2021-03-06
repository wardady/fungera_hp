// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include <iostream>
#include "Fungera.h"

int main(int argc, char *argv[]){
    Fungera{"config.toml"}.run();
    return 0;
}