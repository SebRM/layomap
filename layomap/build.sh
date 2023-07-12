#!/bin/bash

# This script is used to build the OpenOrienteering Mapper program form source.
# it creates a build directory and runs cmake, then builds the project with entrypoint 
# from the main.c++ file in layomap.
# The program is outputted as ./layomap/build/layomap/layomap

# Create build directory and cd into it.
rm build -r -f
mkdir build
cd build

# Run cmake to generate Makefiles
cmake ../..

# cd into the build/layomap directory to 'make' there.
cd layomap

# Prompt user to make the program.
read -p $'\nDo you wish to run the "make" command to build the program? (y/n)\nThis step takes a long ass time.\n' yn
while true; do
  case "$yn" in
      Y|y ) make; break;;
      N|n ) exit;;
      * ) echo "Invalid answer. (y/n)"; read yn;;
  esac
done

echo "Succesfully built program."