#!/bin/bash

# Script para rodar todo o processo

./make.sh

./main.exe arquivos_teste 2>&1 | tee run.log 
# ./main.exe arquivos_teste/teste_0_abertura_arquivo 2>&1 | tee run0.log 
# ./main.exe arquivos_teste/teste_1_um_pedido_por_arquivo 2>&1 | tee run1.log 
# ./main.exe arquivos_teste/teste_2_dois_pedidos_por_arquivo/ 2>&1 | tee run2.log