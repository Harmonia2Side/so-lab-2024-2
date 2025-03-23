#!/bin/bash

# Script para rodar todo o processo

./make.sh

./main.exe arquivos_teste 2>&1 | tee run.log 