#!/bin/bash

# Check if libncurses5-dev is installed
dpkg -l | grep libncurses5-dev > /dev/null
if [ $? -ne 0 ]; then
    echo "Required package 'libncurses5-dev' is not installed."
    echo "Please install it by running the following command:"
    echo "sudo apt-get install libncurses5-dev"
    exit 1
fi

echo "All required packages are installed. You can compile your code."
