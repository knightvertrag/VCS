# IMPERIUM

## Version Control System

This project is a part of the 10 days of Code event organized by the GNU/LINUX USERS' GROUP, NIT Durgapur.

## Setup the project

Clone the project

```bash
git clone https://github.com/knightvertrag/VCS
```

Run the following command

```bash
cd scripts && ./build.sh

```

`build.sh` makes the project and creates an imperium folder in the `$HOME` folder. This folder will contain the compiled C++ file that is executed everytime an imperium command is called. The `imperium.sh` file is also copy pasted over to the folder for convenience. After the files are copied, the `imperium.sh` file is sourced to the `~/.bashrc` file so that its functions can be called from any directory.

## Test the commands

Create or move to an existing directory and type `imperium init`. This will initialize an imperium repository. Now you can play around with the commands listed below

```bash
imperium add [file/folder/.]
imperium commit "[message]"
imperium commit-log
imperium checkout [hash]
imperium revert [hash]
```
