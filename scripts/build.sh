#!/bin/bash

# Move to the root directory of the project (assuming the script is in the scripts/ directory)
cd ..

# Compile the msh executable with source files in src and include headers from include directory
gcc -I./include/ -o ./bin/msh src/*.c

# Check if the compilation succeeded
if [ $? -eq 0 ]; then
    echo "Build successful. msh executable is in the bin directory."
else
    echo "Build failed."
fi
