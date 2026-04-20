#include "parser.h" // Importamos nuestro nuevo archivo
#include "raylib.h"
#include "raymath.h"

int main() {
  const int screenWidth = 1280;
  const int screenHeight = 720;
  InitWindow(screenWidth, screenHeight, "Visor 3D Ligero - aLoonz");

  // Cargamos el modelo ANTES de iniciar el bucle principal
  Modelo3D espada = cargar_obj("Sword_test.obj");

  Camera3D camera = {};
  // Alejamos la cámara drásticamente por el tamaño del modelo
  camera.position = (Vector3){15.0f, 15.0f, 15.0f};
  camera.target = (Vector3){0.0f, 2.0f, 0.0f};
  camera.up = (Vector3){0.0f, 1.0f, 0.0f};
  camera.fovy = 45.0f;
  camera.projection = CAMERA_PERSPECTIVE;

  SetTargetFPS(144);

  // Tu color favorito #3272AC convertido a formato RGBA de Raylib
  Color colorMalla = {50, 114, 172, 255};

  float escala = 0.01f;

  bool modoSolido = false;

  while (!WindowShouldClose()) {
    UpdateCamera(&camera, CAMERA_ORBITAL);

    // Detectar si el usuario presiona la barra espaciadora
    if (IsKeyPressed(KEY_SPACE)) {
      modoSolido = !modoSolido; // Alternar el estado
    }

    BeginDrawing();
    ClearBackground(DARKGRAY);

    BeginMode3D(camera);

    // Recorremos los índices de 3 en 3 (porque ahora son triángulos)
    for (size_t i = 0; i < espada.caras_indices.size(); i += 3) {
      Vector3 v1 =
          Vector3Scale(espada.vertices[espada.caras_indices[i]], escala);
      Vector3 v2 =
          Vector3Scale(espada.vertices[espada.caras_indices[i + 1]], escala);
      Vector3 v3 =
          Vector3Scale(espada.vertices[espada.caras_indices[i + 2]], escala);

      if (modoSolido) {
        // Modo Sólido: Dibujar la cara rellena
        DrawTriangle3D(v1, v2, v3, LIGHTGRAY);

        // Truco visual: Dibujar el borde ligeramente más oscuro para distinguir
        // los polígonos
        DrawLine3D(v1, v2, GRAY);
        DrawLine3D(v2, v3, GRAY);
        DrawLine3D(v3, v1, GRAY);
      } else {
        // Modo Alámbrico original
        DrawLine3D(v1, v2, colorMalla);
        DrawLine3D(v2, v3, colorMalla);
        DrawLine3D(v3, v1, colorMalla);
      }
    }

    DrawGrid(20, 1.0f); // Rejilla más grande para referencia
    EndMode3D();

    DrawFPS(10, 10);
    if (modoSolido) {
      DrawText("Visor 3D - Modo: Solido (Presiona ESPACIO para cambiar)", 10,
               40, 20, LIGHTGRAY);
    } else {
      DrawText("Visor 3D - Modo: Alambrico (Presiona ESPACIO para cambiar)", 10,
               40, 20, colorMalla);
    }

    EndDrawing();
  }

  CloseWindow();
  return 0;
}
