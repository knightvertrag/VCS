#!/bin/bash
function imperium(){
DIR=$PWD
cd ~/imperium/bin || echo "Error"
./main "$1"
cd $DIR
}