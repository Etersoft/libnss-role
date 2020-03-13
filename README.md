# libnss-role

## Contents

* [Introduction](#introduction)
* [Dependencies](#dependencies)
* [Build](#build)

* * *

## Introduction

**libnss-role** is libc module to provide functionality analogous to
Windows' "Restricted Groups".

## Dependencies

* **CMake 3.10+** for building
* **cmocka** for unit testing

## Build

The build process is as simple as:

```
mkdir build
cd build
cmake ..
make
make test
```

