#include "parser.h"
#include <fstream>
#include <iostream>
#include <sstream>

Modelo3D cargar_obj(const std::string &ruta_archivo) {
  Modelo3D modelo;
  std::ifstream archivo(ruta_archivo);

  if (!archivo.is_open()) {
    std::cerr << "Error: No se pudo abrir el archivo " << ruta_archivo
              << std::endl;
    return modelo;
  }

  std::string linea;
  while (std::getline(archivo, linea)) {
    if (linea.empty() || linea[0] == '#')
      continue;

    std::istringstream stream_linea(linea);
    std::string tipo;
    stream_linea >> tipo;

    if (tipo == "v") {
      Vector3 vertice;
      stream_linea >> vertice.x >> vertice.y >> vertice.z;
      modelo.vertices.push_back(vertice);
    } else if (tipo == "f") {
      std::vector<int> vertices_cara;
      std::string datos_vertice;

      while (stream_linea >> datos_vertice) {
        size_t pos_slash = datos_vertice.find('/');
        std::string id_str = datos_vertice.substr(0, pos_slash);
        vertices_cara.push_back(std::stoi(id_str) - 1);
      }

      // Triangulación en abanico (Fan Triangulation)
      // Convierte polígonos de N lados en triángulos
      if (vertices_cara.size() >= 3) {
        for (size_t i = 1; i < vertices_cara.size() - 1; ++i) {
          modelo.caras_indices.push_back(vertices_cara[0]);
          modelo.caras_indices.push_back(vertices_cara[i]);
          modelo.caras_indices.push_back(vertices_cara[i + 1]);
        }
      }
    }
  }

  archivo.close();
  std::cout << "Modelo cargado exitosamente. Vértices: "
            << modelo.vertices.size() << std::endl;
  return modelo;
}
