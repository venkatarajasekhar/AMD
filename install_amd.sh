#!/bin/bash  
# How to run this script from a url: bash <(curl -L -s www.link.com/install_amd.sh)

# # Clone the repo and switch to the "cornell" branch
git clone https://github.com/pkambadu/AMD.git
cd AMD
git checkout cornell

# # Install AMD with all dependencies
./waf configure --install-deps --bld-AMD --tests --bld-examples
./waf

# Include the AMD shared object in the library path
printf "\nexport LD_LIBRARY_PATH=\"%s:%s/build/AMD\"\n" '$LD_LIBRARY_PATH' "$PWD" >> ~/.bashrc 
