#include "parser.h"
#include "raylib.h"
#include "raymath.h"
#include <cmath>
#include <string>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#pragma GCC diagnostic ignored "-Wunused-parameter"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#pragma GCC diagnostic pop

int main() {
  // Agregamos la bandera FLAG_WINDOW_ALWAYS_RUN para que no se pause si salimos
  // de la ventana
  SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_WINDOW_ALWAYS_RUN |
                 FLAG_MSAA_4X_HINT);
  InitWindow(1280, 720, "Visor 3D Ligero - aLoonz");

  Font miFuente =
      LoadFontEx("./JetBrainsMono/JetBrainsMonoNerdFont-Bold.ttf", 18, 0, 0);
  GuiSetFont(miFuente); // Aplica la fuente a los botones de RayGUI

  // Se inicializa un "modelo vacio"
  Modelo3D modeloActual;
  bool modeloCargado = false;

  Camera3D camera = {};
  camera.position = (Vector3){15.0f, 15.0f, 15.0f};
  camera.target = (Vector3){0.0f, 2.0f, 0.0f};
  camera.up = (Vector3){0.0f, 1.0f, 0.0f};
  camera.fovy = 45.0f;
  camera.projection = CAMERA_PERSPECTIVE;

  SetTargetFPS(144);

  Color colorMalla = {50, 114, 172, 255};
  float escala = 1.0f; // Escala dinamica dado el Bounding Box
  bool modoSolido = false;

  // --- CONFIGURACIÓN DE LUZ (Tema 2.4.2) ---
  // Definimos una dirección de luz estática que viene desde arriba y un lado
  Vector3 lightDir = Vector3Normalize({0.5f, 1.0f, 0.5f});

  GuiSetStyle(DEFAULT, TEXT_SIZE, 14);

  while (!WindowShouldClose()) {

    // --- DRAG & DROP y Auto-escalado ---
    if (IsFileDropped()) {
      FilePathList archivosSoltados = LoadDroppedFiles();
      if (archivosSoltados.count > 0) {
        std::string rutaArchivo = archivosSoltados.paths[0];
        if (IsFileExtension(rutaArchivo.c_str(), ".obj")) {
          // Cargamos el nuevo modelo sobrescribiendo el anterior
          modeloActual = cargar_obj(rutaArchivo);
          modeloCargado = true;

          // ALGORITMO DE AUTO-ESCALADO (Bounding Box)
          float max_val = 0.01f;
          for (const auto &v : modeloActual.vertices) {
            if (std::fabs(v.x) > max_val)
              max_val = std::fabs(v.x);
            if (std::fabs(v.y) > max_val)
              max_val = std::fabs(v.y);
            if (std::fabs(v.z) > max_val)
              max_val = std::fabs(v.z);
          }
          escala = 15.0f / max_val;
        }
      }
      UnloadDroppedFiles(archivosSoltados);
    }

    UpdateCamera(&camera, CAMERA_ORBITAL);
    if (IsKeyPressed(KEY_SPACE)) {
      modoSolido = !modoSolido;
    }

    BeginDrawing();
    ClearBackground(DARKGRAY);

    BeginMode3D(camera);
    if (modeloCargado) {
      for (const auto &cara : modeloActual.caras) {
        Vector3 v1 = Vector3Scale(modeloActual.vertices[cara.v1], escala);
        Vector3 v2 = Vector3Scale(modeloActual.vertices[cara.v2], escala);
        Vector3 v3 = Vector3Scale(modeloActual.vertices[cara.v3], escala);
        if (modoSolido) {
          // CÁLCULO DE ILUMINACIÓN DIFUSA (Lambert)
          // El producto punto nos da el coseno del ángulo entre normal y luz
          float dot = Vector3DotProduct(cara.normal, lightDir);
          if (dot < 0.0f)
            dot = 0.0f; // Evitamos luz negativa

          // Intensidad = (Difusa * dot) + Ambiental (0.3 para no ver negro
          // total)
          float intensidad = (dot * 0.7f) + 0.3f;

          Color colorFinal = {(unsigned char)(180 * intensidad),
                              (unsigned char)(180 * intensidad),
                              (unsigned char)(180 * intensidad), 255};

          DrawTriangle3D(v1, v2, v3, colorFinal);

          // Líneas de contorno sutiles para resaltar la geometría
          DrawLine3D(v1, v2, (Color){30, 30, 30, 100});
          DrawLine3D(v2, v3, (Color){30, 30, 30, 100});
          DrawLine3D(v3, v1, (Color){30, 30, 30, 100});
        } else {
          DrawLine3D(v1, v2, colorMalla);
          DrawLine3D(v2, v3, colorMalla);
          DrawLine3D(v3, v1, colorMalla);
        }
      }
    }

    DrawGrid(20, 1.0f);
    EndMode3D();

    // Interface responsiva
    int w = GetScreenWidth();
    int h = GetScreenHeight();

    // Botón y Textos siempre visibles
    DrawFPS(10, 10);
    const char *textoModo =
        modoSolido ? "Modo Actual: Solido" : "Modo Actual: Alambrico";
    DrawText(textoModo, 10, 35, 14, modoSolido ? LIGHTGRAY : colorMalla);

    // El botón se ancla al ancho dinámico menos un margen
    if (GuiButton((Rectangle){(float)w - 170, 10, 150, 25}, "Alternar Modo")) {
      modoSolido = !modoSolido;
    }

    // Mensaje centrado si no hay modelo, usando las dimensiones dinámicas
    if (!modeloCargado) {
      const char *msg = "Arrastra un archivo .obj a esta ventana";
      int msgWidth = MeasureText(msg, 20);
      DrawText(msg, (w - msgWidth) / 2, h / 2, 20, LIGHTGRAY);
    }

    EndDrawing();
  }

  CloseWindow();
  return 0;
}
