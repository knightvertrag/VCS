#!/bin/bash
echo "export PATH=\$PATH:$PWD" >> ~/.bashrc
mkdir -p ~/imperium/bin
cd ..
make
cd ~/imperium/bin || echo "error"
chmod +x main


