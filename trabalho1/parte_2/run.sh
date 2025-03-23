#!/bin/bash

# Script para rodar todo o processo

./make.sh

./main.exe 2>&1 | tee run.log