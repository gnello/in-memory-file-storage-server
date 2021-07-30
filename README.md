# File Storage Server
[![C/C++ CI][1]][2]

Simple In Memory File Storage Server case study.  

Concepts involved: data-structures, concurrency (multi-thread), locks, condition variables, pipes, sockets, 
master-worker design pattern, signals handling, file-system and volume. 

This project it is far to be perfect, any suggestions or criticisms are welcome. Open an issue to suggest improvements 
or to argue about something that does not add up to you, and I will be happy to answer.

## Installation
This project is totally conform to POSIX, this means that you do not need any extra library to use it
except for those already present in your operating system.
To install the project it is sufficient to follow the following simple commands:

```bash
git clone git@github.com:gnello/in-memory-file-storage-server.git # clone the project
cd in-memory-file-storage-server/ # move into the project directory
make
```

## Server
When you start the server, you need to pass a configuration file to it. You can edit the server configuration by editing 
the config.txt file in the root of the project, or you can create a new one. Then you just need to provide the correct 
configuration file to the server when you start it.  

To start the server run the following command in a terminal from the root of the project:

```bash
make server # build the server files 
cd server/ # move into the server directory
./main -f ../config.txt # run the server
```

The server requires and accepts only one option to start: the configuration file. The usage is shown below: 

```text
Usage: main [options]
Start the In Memory Storage Server.
Example: main -f ./config.txt

  -h                          Print this message and exit.
  -f FILENAME                 Start the server with the FILENAME configuration file.
```

## Client
This project provides a simple client that makes you able to use the server. The client connection to the server is 
active for as long as it takes to process the options provided, when all the options have been processed the connection 
is close.  

The usage is shown below:

```text
Usage: main [options]
Write and read files to and from the In Memory Storage Server.
Example: main -f /tmp/fss.sk -r file1 -d /dev/null -w ./mydir

The order of the options matters, the options will be handled in the order
they appear.
The -f, -h, -p options can not be specified more than once.
The -f option must be always specified.

  -h                          Print this message and exit.
  -f FILENAME                 Connect to the FILENAME socket.
  -w DIRNAME[,N=0]            Send DIRNAME files to the Server.
                              If N is specified, send N files of DIRNAME
                              to the Server.
  -W FILE1[,FILE2...]         Send any given FILE/s to the Server.
  -D DIRNAME                  Store the files trashed by the Server into DIRNAME.
                              It must be followed by a -w or -W option.
  -r FILE1[,FILE2...]         Read FILE/s from the Server.
  -R [N=0]                    Read all files stored on the Server.
                              If N is specified, read N random files from
                              the Server.
  -d DIRNAME                  Store the files read from the Server into DIRNAME.
                              It must be followed by a -r or -R option.
  -t TIME                     Wait TIME milliseconds between sequential requests
                              to the Server.
  -l FILE1[,FILE2...]         Acquire the lock on FILE/s.
  -u FILE1[,FILE2...]         Release the lock on FILE/s.
  -c FILE1[,FILE2...]         Remove FILE/s from the Server.
  -p                          Print the log of the requests made to the server.
```

## Compiling:

Target | Command | Description
--- | --- | ---
all | `make`| Compiles the project
dev | `make dev`| Compiles the project with also the test suites
client | `make client`| Compiles the client
server | `make server`| Compiles the server
helpers | `make helpers`| Compiles the helpers library in `helpers/lib`
data-structures | `make data-structures`| Compiles the data-structures library in `data-structures/lib`
message | `make message`| Compiles the message library in `message/lib`
socket | `make socket`| Compiles the socket library in `socket/lib`
storage | `make storage`| Compiles the storage library in `storage/lib`
tests | `make tests`| Runs all the test suites
tests-valgrind | `make tests-valgrind`| Runs all the test suites with valgrind
clean | `make clean`| Clean all the executable, library and object files
clean-dev | `make clean-dev`| Clean all the executable, library and object files, including tests

[1]: https://github.com/gnello/so-project/actions/workflows/c-cpp.yml/badge.svg
[2]: https://github.com/gnello/so-project/actions/workflows/c-cpp.yml
