# Fungera_hp
> More efficient implementation of truly two-dimensional artificial life simulator [Fungera](https://github.com/mxpoliakov/fungera) written in C++

## Table of contetnts
  - [Requirements](#requirements)
  - [Building](#building)
  - [Description](#description)
    - [Simulation structure](simulation-structure)
    - [GUI](gui)
    - [Snapshots](snapshots)
  - [Credits](credits)

## Requirements
- [Qt](https://www.qt.io/) 5.15.2 or newer
- [Boost](https://www.boost.org/) 1.71 or newer
- [CMake](https://cmake.org/) 3.13 or newer

## Building
```
mkdir build && cd build
cmake -G"Unix Makefiles" ..
make
./fungera --help
```

## Description
The description of Fungera's ISA can be found in the [reference implementation](https://github.com/mxpoliakov/fungera).
Here are covered aspects of the current implementation.

__*Fungera_hp*__ is the implementation of the Fungera simulator in C++, with better performance.
This was achieved by eliminating some algorithmic flaws which were present in the reference implementation.
There were changes in architecture done as well. GUI was designed to be a separate essence from the very beginning and use simulation as the source of data.
This allows executing the simulation without the GUI with little to no drawbacks brought by the GUI feature. Graphical Interfaces are needed mainly for debugging purposes.

### Simulation structure
Basic description of the implemented simulatior structure.

<img src="https://user-images.githubusercontent.com/36009082/120267578-e8278680-c2ac-11eb-9135-275fec56559b.png">

Fungera - the central object, which manages all other parts.
It is responsible for the execution, creates, and owns Memory and Queue instances.
Fungera object is also the main interface of communication with the whole simulation. 
Memory object represents RAM and provides an interface to interact with it.
It holds memory cells, each cell containing one machine instruction and a flag that indicates if some organism occupies the cell.
Memory provides an interface for allocation/deallocation of the memory block, searching for a free block of the requested size, memory protection.
Queue object is the owner of organisms. It is responsible for providing real CPU time for all creatures.
Organism object holds Fungera’s organism structure along with the implementation of the instruction set.
It also implements different error tracking for different commands which allows more flexible modification of the instruction set.


### GUI
> Currently in branch feature-GUI

GUI provides ability to visualize and troubleshoot the simulation.
Currently supported features:
- Memory visualisation:
  - All memory cells contetns
  - Highlighting all alive organisms & organism's borders
  - Highlighting selected organism
  - Highlighting selected organism's instruction pointer
- General inforamation about the simulation (cycle, number of alive organisms, etc).
- Selected organism status
- Control panel

<img src="https://user-images.githubusercontent.com/36009082/120267574-e65dc300-c2ac-11eb-8985-a0e4cb4ff093.gif">

### Snapshots
In order to save the information about the state of the simulation for the restarts of the simulation or further analysis and obtain information about existing genotypes and other aspects that can be valuable, snapshots are saved.
The implemented solution is using the Boost Serialization library.
__*Fungera_hp*__ uses two types of archives:
- portable text archives - to save the state of the simulation on disk
- XML archives - in order to represent information in more convinient format for further research
Current implementation includes functionality to convert text archives to XML archives.

## Credits
[Oleg Farenyuk](https://apps.ucu.edu.ua/en/personal/oleg-farenyuk/) - Supervisor
