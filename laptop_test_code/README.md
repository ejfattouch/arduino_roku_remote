# Laptop Test Code for Roku Discovery

This directory contains the prototype C++ code intended for testing Roku device discovery functionality on a laptop before 
porting it to Arduino. It leverages ASIO for network communication to send a Simple Service Discovery Protocol (SSDP) multicast
request to locate Roku devices on the local network.

## Requirements

- ASIO Library: This code uses ASIO for asynchronous network operations.
- C++11 or higher: Ensures support for modern language features.

## Compilation and Setup

This project uses CMake to manage the build process. Ensure that CMake is installed on your system.

Steps to Compile

1. Navigate to the directory containing the CMakeLists.txt file.
2. Run the following commands:
```shell
mkdir build
cd build
cmake ..
make
```

This will generate an executable named `RokuDiscovery` in the build directory.

## Running the Program

After building, you can run the program with:
```shell
./RokuDiscovery
```


## Contents

### `main.cpp`
This file runs the tests we want to perform on a laptop before implementing them in the Arduino IDE

### `rokuIPDiscovery.cpp`
This file contains handles tasks related to querying the network and finding the IP address of a roku device)

### `rokuSendCommand.cpp`
This file provides a class `RokuCommandHandler` which handles sending commands 
as http POST requests to emulate button presses on a Roku Remote