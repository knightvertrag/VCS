#!/bin/bash
mkdir -p ~/imperium/bin
cd ..
make
cd ~/imperium/bin || echo "error"
chmod +x main


