#include "parser.h"
#include "raymath.h"
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

      // Triangulación en abanico y cálculo de NORMALES
      if (vertices_cara.size() >= 3) {
        for (size_t i = 1; i < vertices_cara.size() - 1; ++i) {
          Cara nueva_cara;
          nueva_cara.v1 = vertices_cara[0];
          nueva_cara.v2 = vertices_cara[i];
          nueva_cara.v3 = vertices_cara[i + 1];

          // Obtenemos los 3 puntos en el espacio 3D
          Vector3 p1 = modelo.vertices[nueva_cara.v1];
          Vector3 p2 = modelo.vertices[nueva_cara.v2];
          Vector3 p3 = modelo.vertices[nueva_cara.v3];

          // 1. Calculamos dos vectores en el plano del triángulo
          Vector3 vectorA = Vector3Subtract(p2, p1);
          Vector3 vectorB = Vector3Subtract(p3, p1);

          // 2. Calculamos el Producto Cruz para obtener el vector perpendicular
          Vector3 normal_calculada = Vector3CrossProduct(vectorA, vectorB);

          // 3. Normalizamos el vector (longitud = 1)
          nueva_cara.normal = Vector3Normalize(normal_calculada);

          // Guardamos la cara ya con su normal calculada
          modelo.caras.push_back(nueva_cara);
        }
      }
    }
  }

  archivo.close();
  std::cout << "Modelo cargado exitosamente. Vértices: "
            << modelo.vertices.size()
            << " | Caras (Triángulos): " << modelo.caras.size() << std::endl;
  return modelo;
}
