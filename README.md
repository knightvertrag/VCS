# IMPERIUM

## Version Control System

A version control systems trying to replicate what git does.

## Setup the project

The project uses cmake to generate its build system and openssl for SHA1 hashing, so you will need these installed on your system 
before you build the executable.

Install cmake and openssl

```bash
sudo apt install cmake libssl-dev
```

Clone the project

```bash
git clone https://github.com/knightvertrag/VCS
```
Execute the following commands to build the binary

```bash
mkdir build
cd build
cmake ../
cmake --build .
```

A binary `imperium` will be built, which you can now use to version control.

Run the following command to display all the available commands

```bash
./imperium -h
```