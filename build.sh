#!/bin/bash

# Nombre del archivo ejecutable que se va a generar
OUTPUT="visor3D"

# Archivos fuente (si después creas un parser.cpp, lo añades aquí)
SOURCES="main.cpp parser.cpp"

# Banderas del compilador:
# -std=c++17: Usa un estándar moderno de C++
# -Wall -Wextra: Activa las advertencias para que te avise si hay código dudoso
# -O2: Optimiza el rendimiento del binario
CXXFLAGS="-std=c++17 -Wall -Wextra -O2"

# Banderas de enlace usando pkg-config (la magia para no pelear con dependencias)
LDFLAGS=$(pkg-config --libs --cflags raylib)

echo "⚙️  Compilando el proyecto..."

# El comando real de compilación usando g++
g++ $SOURCES -o $OUTPUT $CXXFLAGS $LDFLAGS

# Comprobar si la compilación fue exitosa
if [ $? -eq 0 ]; then
  echo "✅ Compilación exitosa."
  echo "🚀 Ejecutando el visor..."
  ./$OUTPUT
else
  echo "❌ Error en la compilación. Revisa el código de arriba."
fi
