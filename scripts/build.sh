#!/bin/bash
mkdir -p ~/imperium/bin
cp imperium.sh ~/imperium
cd ..
make
cd ~/imperium/bin || echo "error"
chmod +x main
cd ..
echo "source $PWD/imperium.sh" >> ~/.bashrc



