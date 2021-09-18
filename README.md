# IMPERIUM

## Version Control System

A version control systems trying to replicate what git does.

## Setup the project

Clone the project and switch to the overhaul branch

```bash
git clone https://github.com/knightvertrag/VCS
git checkout overhaul
```

The project is using Boost for Zlib compression and SHA. We will pobably replace it with a lighter solution, but for now you will have to set up Boost. 

https://www.boost.org/doc/libs/1_75_0/more/getting_started/unix-variants.html

Follow the above instructions to set up Boost. We are using the Iostreams library which requires binaries, so build them accordingly, and place them in a `lib` folder in the project root. Copy the `boost` folder from the downloaded archive and place it inside an `include` folder, also in the project root.


Once the project directory has been set up, run the following command to build the binary in the `bin` directory. A prebuilt binary is already provided in the `bin` directory.

```bash
make
```

Run the following commands to display all the available commands

```bash
cd bin
./imperium -h
```