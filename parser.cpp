#include "parser.h"
#include "raymath.h"
#include <assimp/Importer.hpp>  // Interfaz del importador de C++
#include <assimp/postprocess.h> // Banderas de post-procesamiento
#include <assimp/scene.h>       // Estructura de salida de Assimp
#include <iostream>

Modelo3D cargar_modelo(const std::string &ruta_archivo) {
  Modelo3D modelo;
  Assimp::Importer importer;

  // - aiProcess_Triangulate: Convierte cualquier N-ágono en triángulos.
  // - aiProcess_JoinIdenticalVertices: Optimiza la memoria de la malla.
  const aiScene *scene = importer.ReadFile(
      ruta_archivo, aiProcess_Triangulate | aiProcess_JoinIdenticalVertices);

  if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE ||
      !scene->mRootNode) {
    std::cerr << "Error al cargar modelo con Assimp: "
              << importer.GetErrorString() << std::endl;
    return modelo;
  }

  // Mesh del archivo
  if (scene->mNumMeshes > 0) {
    aiMesh *mesh = scene->mMeshes[0];

    // Verificamos si el modelo contiene mapas UV
    bool tieneUVs = mesh->HasTextureCoords(0);

    // 1. Extraer los vértices
    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
      Vector3 vertice;
      vertice.x = mesh->mVertices[i].x;
      vertice.y = mesh->mVertices[i].y;
      vertice.z = mesh->mVertices[i].z;
      modelo.vertices.push_back(vertice);
    }

    // 2. Extraer las caras y calcular la Normal
    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
      aiFace face = mesh->mFaces[i];

      // Al activar aiProcess_Triangulate, estamos seguros de que cada cara
      // tiene 3 índices
      if (face.mNumIndices == 3) {
        Cara nueva_cara;
        nueva_cara.v1 = face.mIndices[0];
        nueva_cara.v2 = face.mIndices[1];
        nueva_cara.v3 = face.mIndices[2];

        // Lambert
        Vector3 p1 = modelo.vertices[nueva_cara.v1];
        Vector3 p2 = modelo.vertices[nueva_cara.v2];
        Vector3 p3 = modelo.vertices[nueva_cara.v3];

        Vector3 vectorA = Vector3Subtract(p2, p1);
        Vector3 vectorB = Vector3Subtract(p3, p1);
        Vector3 normal_calculada = Vector3CrossProduct(vectorA, vectorB);

        nueva_cara.normal = Vector3Normalize(normal_calculada);

        // Extraemos las coordenadas 2D si existen en el modelo
        if (tieneUVs) {
          nueva_cara.uv1 = {mesh->mTextureCoords[0][nueva_cara.v1].x,
                            mesh->mTextureCoords[0][nueva_cara.v1].y};
          nueva_cara.uv2 = {mesh->mTextureCoords[0][nueva_cara.v2].x,
                            mesh->mTextureCoords[0][nueva_cara.v2].y};
          nueva_cara.uv3 = {mesh->mTextureCoords[0][nueva_cara.v3].x,
                            mesh->mTextureCoords[0][nueva_cara.v3].y};
        } else {
          nueva_cara.uv1 = {0.0f, 0.0f};
          nueva_cara.uv2 = {0.0f, 0.0f};
          nueva_cara.uv3 = {0.0f, 0.0f};
        }

        modelo.caras.push_back(nueva_cara);
      }
    }
    std::cout << "[Assimp] Modelo cargado: " << ruta_archivo << "\n"
              << "Vértices: " << modelo.vertices.size()
              << " | Caras: " << modelo.caras.size() << std::endl;
  } else {
    std::cerr << "El archivo no contiene mallas 3D validas." << std::endl;
  }

  return modelo;
}
