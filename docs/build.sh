#!/bin/bash

# sudo apt install doxygen graphviz texlive-latex-base texlive-latex-recommended texlive-pictures texlive-latex-extra

set -e # Exit immediately if a command exits with a non-zero status.

function setup_virtualenv()
{
    if [ ! -d ".venv" ]; then
        virtualenv .venv
        source .venv/bin/activate
        if [ -f "requirements.txt" ]; then
            pip3 install -r requirements.txt
        fi
        deactivate
    fi

    source .venv/bin/activate
}

function setup_doxygen()
{
    ( # setup doxygen awesome https://jothepro.github.io/doxygen-awesome-css/
        cd doxygen-awesome-css
        git checkout v2.3.3
    )
}

    
function generate_doxygen()
{
    if [ ! -f Doxyfile ]; then
        return
    fi 

    doxygen Doxyfile # Run Doxygen

#     make -C latex # Generate Doxygen PDF
#     cp latex/refman.pdf ./output.pdf # Copy PDF to root directory
}

function generate_sphinx()
{
    doxysphinx build sphinx output Doxyfile # generate RST files for sphinx from doxygen's output
    sphinx-build -M html sphinx/ html # generate sphinx output
}

setup_virtualenv
setup_doxygen
generate_doxygen
generate_sphinx
