#!/bin/bash
mkdir -p ~/imperium/bin
cd ..
make
cd ~/imperium/bin || echo "error"
chmod +x main
echo "export PATH=""$HOME"/imperium/bin:"$PATH""" >> ~/.bashrc

