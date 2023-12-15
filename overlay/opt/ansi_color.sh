#!/bin/bash

green() {
	echo -ne "\033[32m"
}

red() {
	echo -ne "\033[31m"
}

blue() {
	echo -ne "\033[34m"
}

nocolor() {
	echo -ne "\033[0m"
}

red
echo -n "R"
green
echo -n "G"
blue
echo -n "B"
nocolor
