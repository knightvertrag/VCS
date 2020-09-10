#!/bin/bash
function imperium(){
    DIR=$PWD
    export dir=$DIR
    cd ~/imperium/bin || echo "Error"
    ./main "$1"
    cd "$DIR" || echo "Error"
}