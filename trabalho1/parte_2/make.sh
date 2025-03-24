#!/bin/bash

# Script para rodar todo o processo

# exit when any command fails - Ajuda o script a ter segurança jurídica
set -e

# Limpa compilação anterior
make clean

# gera debug
make debug

#Compila tudo e imprime o resultado num log
make all 2>&1 | tee make.log