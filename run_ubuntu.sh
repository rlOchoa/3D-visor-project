#!/bin/bash

echo "========================================"
echo "Instalador y Compilador - Visor3D (Ubuntu)"
echo "========================================"
echo ""
echo "Se solicitará contraseña de administrador para instalar dependencias (raylib y assimp)..."

sudo apt update
sudo apt install -y build-essential libraylib-dev libassimp-dev pkg-config

echo ""
echo "⚙️  Compilando el proyecto..."

g++ main.cpp parser.cpp -o visor3D -std=c++17 -Wall -Wextra -O2 $(pkg-config --libs --cflags raylib) -lassimp

if [ $? -eq 0 ]; then
  echo "✅ Compilación exitosa."
  echo "🚀 Abriendo Visor3D..."
  ./visor3D
else
  echo "❌ Error en la compilación."
fi
