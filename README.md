# SO project

[![C/C++ CI](https://github.com/gnello/so-project/actions/workflows/c-cpp.yml/badge.svg)](https://github.com/gnello/so-project/actions/workflows/c-cpp.yml)

## Usage
Build the project with the following command
```shell
make
```
then you can start the program
```shell
cd dist/ && ./main
```

## Tests
Build the project with the following command
```shell
make dev
```
then you can run the tests
```shell
make tests
```

### Valgrind
To run the tests with valgrind simply run the following command
```shell
make tests-valgrind
```

## Clean
To clean the project run
```shell
make clean
```
If you built the project using `make dev` run
```shell
make clean-dev
```
