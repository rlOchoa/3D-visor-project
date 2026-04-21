#include "parser.h" // Importamos nuestro nuevo archivo
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
  const int screenWidth = 1280;
  const int screenHeight = 720;
  // Agregamos la bandera FLAG_WINDOW_ALWAYS_RUN para que no se pause si salimos
  // de la ventana
  SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_WINDOW_ALWAYS_RUN);
  InitWindow(screenWidth, screenHeight, "Visor 3D Ligero - aLoonz");

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

  // Configuración visual básica para que el botón no desentone con tu fondo
  GuiSetStyle(DEFAULT, TEXT_SIZE, 14);

  while (!WindowShouldClose()) {

    // --- DRAG & DROP y Auto-escalado ---
    if (IsFileDropped()) {
      FilePathList archivosSoltados = LoadDroppedFiles();

      // Verificamos que soltaron al menos un archivo
      if (archivosSoltados.count > 0) {
        std::string rutaArchivo = archivosSoltados.paths[0];

        // Verificamos que la extensión sea .obj (Raylib tiene una función para
        // esto)
        if (IsFileExtension(rutaArchivo.c_str(), ".obj")) {
          // Cargamos el nuevo modelo sobrescribiendo el anterior
          modeloActual = cargar_obj(rutaArchivo);
          modeloCargado = true;

          // ALGORITMO DE AUTO-ESCALADO (Bounding Box)
          // Encontramos la coordenada más extrema del modelo
          float max_val = 0.0f;
          for (const auto &v : modeloActual.vertices) {
            if (std::fabs(v.x) > max_val)
              max_val = std::fabs(v.x);
            if (std::fabs(v.y) > max_val)
              max_val = std::fabs(v.y);
            if (std::fabs(v.z) > max_val)
              max_val = std::fabs(v.z);
          }

          // Ajustamos la escala para que el modelo más grande mida siempre ~15
          // unidades
          escala = (max_val > 0.0f) ? (15.0f / max_val) : 1.0f;
        }
      }
      // Liberamos la memoria de la lista de rutas
      UnloadDroppedFiles(archivosSoltados);
    }
    // ----

    UpdateCamera(&camera, CAMERA_ORBITAL);

    // Detectar si el usuario presiona la barra espaciadora
    if (IsKeyPressed(KEY_SPACE)) {
      modoSolido = !modoSolido; // Alternar el estado
    }

    BeginDrawing();
    ClearBackground(DARKGRAY);

    BeginMode3D(camera);
    // Solo intentamos dibujar si realmente hay un modelo cargado
    if (modeloCargado) {
      // Recorremos los índices de 3 en 3 (porque ahora son triángulos)
      for (size_t i = 0; i < modeloActual.caras_indices.size(); i += 3) {
        Vector3 v1 = Vector3Scale(
            modeloActual.vertices[modeloActual.caras_indices[i]], escala);
        Vector3 v2 = Vector3Scale(
            modeloActual.vertices[modeloActual.caras_indices[i + 1]], escala);
        Vector3 v3 = Vector3Scale(
            modeloActual.vertices[modeloActual.caras_indices[i + 2]], escala);

        if (modoSolido) {
          DrawTriangle3D(v1, v2, v3, LIGHTGRAY);
          DrawLine3D(v1, v2, GRAY);
          DrawLine3D(v2, v3, GRAY);
          DrawLine3D(v3, v1, GRAY);
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
    // Obtenemos las dimensiones reales de la ventana en este fotograma exacto
    int anchoDinamico = GetScreenWidth();
    int altoDinamico = GetScreenHeight();

    // La interface va a partir de aqui
    DrawFPS(10, 10);

    // Botón y Textos siempre visibles
    DrawFPS(10, 10);
    const char *textoModo =
        modoSolido ? "Modo Actual: Solido" : "Modo Actual: Alambrico";
    DrawText(textoModo, 10, 35, 14, modoSolido ? LIGHTGRAY : colorMalla);

    // El botón se ancla al ancho dinámico menos un margen
    if (GuiButton((Rectangle){(float)anchoDinamico - 170, 10, 150, 25},
                  "Alternar Modo")) {
      modoSolido = !modoSolido;
    }

    // Mensaje centrado si no hay modelo, usando las dimensiones dinámicas
    if (!modeloCargado) {
      const char *msg = "Arrastra un archivo .obj a esta ventana";
      int msgWidth = MeasureText(msg, 20);
      DrawText(msg, (anchoDinamico - msgWidth) / 2, altoDinamico / 2, 20,
               LIGHTGRAY);
    }

    EndDrawing();
  }

  CloseWindow();
  return 0;
}
