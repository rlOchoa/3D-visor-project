#pragma once
#include <vector>
#include <string>
#include "raylib.h"

struct Modelo3D {
    std::vector<Vector3> vertices;
    std::vector<int> caras_indices;
};

Modelo3D cargar_obj(const std::string& ruta_archivo);
