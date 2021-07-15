# File Storage Server
[![C/C++ CI][1]][2]

Simple In Memory File Storage Server case study.  
Concepts involved: data-structures, concurrency (multi-thread), pipes, sockets, master-worker design pattern, signals handling, storage and deep memory management. 

## Installation
This project is completely portable (totally conform to POSIX), this means that you do not need any extra library to use it.
To install the project it is sufficient to follow the following simple commands:

```bash
git clone git@github.com:gnello/in-memory-file-storage-server.git # clone the project
cd in-memory-file-storage-server/ # move into the project directory
```

## Server
To run the server execute the following command from the root of the project:

```bash
make server # build the server files 
cd server/ # move into the server directory
./main -f ../config.txt # run the server
```

To see all the options available do:
```bash
./main -h # show the usage message
```

## Compiling:

- `make`: Compiles the main in `bin/main`
- `make helpers`: Compiles the helpers library in `helpers/lib`
- `make data-structures`: Compiles the data-structures library in `data-structures/lib`
- `make dev`: Compiles the program and the tests
- `make tests`: Runs all the test suites
- `make tests-valgrind`: Runs all the test suites with valgrind
- `make clean`: Clean all the executable, library and object files
- `make clean-dev`: Clean all the executable, library and object files, including tests

[1]: https://github.com/gnello/so-project/actions/workflows/c-cpp.yml/badge.svg
[2]: https://github.com/gnello/so-project/actions/workflows/c-cpp.yml
