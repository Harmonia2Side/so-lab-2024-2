# Compilação

## Usando CMAKE

- Exemplo para o lab5
```
cd ~/codes/so-lab-2024-2
mkdir lab5_build
cd lab5_build/
cmake ../lab5
cmake --build .
```

# Linter

- Usamos a extensão do vscode chamada clangd (https://open-vsx.org/extension/llvm-vs-code-extensions/vscode-clangd).
- Ela possui um setup, disponível em https://clangd.llvm.org/installation#project-setup
- em resumo:
```
cd lab5_build/
cmake ../lab5 -DCMAKE_EXPORT_COMPILE_COMMANDS=1
cd ../lab5
ln -s ../lab5_build/compile_commands.json .
```