#pragma once
#include "raylib.h"
#include <string>
#include <vector>

// Nueva estructura para guardar una cara completa
struct Cara {
  int v1, v2, v3; // Indices de los vertices
  Vector3 normal; // El vector normal calculado para esta cara
};

struct Modelo3D {
  std::vector<Vector3> vertices;
  std::vector<Cara> caras; // Reemplar caras_indices por un arreglo de Caras
};

Modelo3D cargar_modelo(const std::string &ruta_archivo);
