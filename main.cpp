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
  InitWindow(1280, 720, "Visor 3D Ligero - Roman Ochoa Oliva");

  Font miFuente =
      LoadFontEx("./JetBrainsMono/JetBrainsMonoNerdFont-Bold.ttf", 18, 0, 0);
  GuiSetFont(miFuente);

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

  // --- VARIABLES DE ESTADO PARA UI ---
  Color colorMalla = {50, 114, 172, 255};      // Default blue
  Vector3 rotacionManual = {0.0f, 0.0f, 0.0f}; // Grados de rotación X, Y, Z
  float escalaManual = 1.0f;
  float escalaBase = 1.0f; // Escala dinamica dado el Bounding Box
  bool modoSolido = false;

  // --- CONFIGURACIÓN DE LUZ (Tema 2.4.2) ---
  Vector3 lightDir = Vector3Normalize({0.5f, 1.0f, 0.5f});

  GuiSetStyle(DEFAULT, TEXT_SIZE, 14);

  while (!WindowShouldClose()) {

    // --- DRAG & DROP ---
    if (IsFileDropped()) {
      FilePathList archivosSoltados = LoadDroppedFiles();
      if (archivosSoltados.count > 0) {
        std::string rutaArchivo = archivosSoltados.paths[0];
        if (IsFileExtension(rutaArchivo.c_str(), ".obj")) {
          // Cargamos el nuevo modelo sobrescribiendo el anterior
          modeloActual = cargar_obj(rutaArchivo);
          modeloCargado = true;

          // Reset de rotación al cargar un nuevo modelo
          rotacionManual = {0.0f, 0.0f, 0.0f};
          escalaManual = 1.0f;

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
          escalaBase = 15.0f / max_val;
        }
      }
      UnloadDroppedFiles(archivosSoltados);
    }

    UpdateCamera(&camera, CAMERA_ORBITAL);
    if (IsKeyPressed(KEY_SPACE))
      modoSolido = !modoSolido;

    // MATRIZ DE TRANSFORMACIÓN AFÍN (Tema 1.3)
    // Convertimos grados a radianes y generamos la matriz de rotación combinada
    Matrix matRotacion = MatrixRotateXYZ((Vector3){rotacionManual.x * DEG2RAD,
                                                   rotacionManual.y * DEG2RAD,
                                                   rotacionManual.z * DEG2RAD});

    // Escala final combianndo Bounding Box automático y el slider dinámico.
    float escalaFinal = escalaBase * escalaManual;

    BeginDrawing();
    ClearBackground(DARKGRAY);

    BeginMode3D(camera);

    if (modeloCargado) {
      for (const auto &cara : modeloActual.caras) {

        // Vértices originales.
        Vector3 v1 = modeloActual.vertices[cara.v1];
        Vector3 v2 = modeloActual.vertices[cara.v2];
        Vector3 v3 = modeloActual.vertices[cara.v3];

        // Transformación de rotación (Multiplicación por Matriz)
        v1 = Vector3Transform(v1, matRotacion);
        v2 = Vector3Transform(v2, matRotacion);
        v3 = Vector3Transform(v3, matRotacion);

        // Transformación de escala uniforme
        v1 = Vector3Scale(v1, escalaFinal);
        v2 = Vector3Scale(v2, escalaFinal);
        v3 = Vector3Scale(v3, escalaFinal);

        if (modoSolido) {
          // Rotar el vector normal para que la luz reaccione al giro
          Vector3 normalRotada = Vector3Transform(cara.normal, matRotacion);

          // El producto punto nos da el coseno del ángulo entre normal y
          // luz
          float dot = Vector3DotProduct(normalRotada, lightDir);
          if (dot < 0.0f)
            dot = 0.0f; // Evitamos luz negativa

          // Intensidad = (Difusa * dot) + Ambiental (0.3 para no ver negro
          // total)
          float intensidad = (dot * 0.7f) + 0.3f;

          Color colorFinal = {(unsigned char)(colorMalla.r * intensidad),
                              (unsigned char)(colorMalla.g * intensidad),
                              (unsigned char)(colorMalla.b * intensidad),
                              colorMalla.a};

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

    DrawGrid(400, 10.0f);
    EndMode3D();

    // --- PANEL DE CONTROL LATERAL ---
    int w = GetScreenWidth();
    int h = GetScreenHeight();

    DrawFPS(10, 10);
    const char *textoModo =
        modoSolido ? "Modo Actual: Solido" : "Modo Actual: Malla";
    DrawText(textoModo, 10, 35, 14, LIGHTGRAY);

    // Mensaje centrado si no hay modelo, usando las dimensiones dinámicas
    if (!modeloCargado) {
      const char *msg = "Arrastra un archivo .obj a esta ventana";
      int msgWidth = MeasureText(msg, 20);
      DrawText(msg, (w - msgWidth) / 2, h / 2, 20, LIGHTGRAY);
    }

    // Panel anclado a la derecha
    float panelX = w - 240.0f;
    GuiPanel((Rectangle){panelX, 10, 230, 410},
             "Herramientas (Transformaciones)");

    // El botón se ancla al ancho dinámico menos un margen
    if (GuiButton((Rectangle){panelX + 15, 40, 200, 25}, "Alternar Modo")) {
      modoSolido = !modoSolido;
    }

    // Sliders de Rotación Afín
    GuiLabel((Rectangle){panelX + 15, 80, 100, 20}, "Rotacion X");
    GuiSliderBar((Rectangle){panelX + 90, 80, 120, 20}, "",
                 TextFormat("%.0f", rotacionManual.x), &rotacionManual.x, 0.0f,
                 360.0f);

    GuiLabel((Rectangle){panelX + 15, 110, 100, 20}, "Rotacion Y");
    GuiSliderBar((Rectangle){panelX + 90, 110, 120, 20}, "",
                 TextFormat("%.0f", rotacionManual.y), &rotacionManual.y, 0.0f,
                 360.0f);

    GuiLabel((Rectangle){panelX + 15, 140, 100, 20}, "Rotacion Z");
    GuiSliderBar((Rectangle){panelX + 90, 140, 120, 20}, "",
                 TextFormat("%.0f", rotacionManual.z), &rotacionManual.z, 0.0f,
                 360.0f);

    // Nuevo Slider de Escalamiento Manual (Rango de 0.1x a 3.0x el tamaño
    // original)
    GuiLabel((Rectangle){panelX + 15, 165, 100, 20}, "Escala");
    GuiSliderBar((Rectangle){panelX + 90, 165, 120, 20}, "",
                 TextFormat("%.2fx", escalaManual), &escalaManual, 0.1f, 3.0f);

    // Selector de Color
    GuiLabel((Rectangle){panelX + 15, 180, 100, 20}, "Color del Modelo:");
    GuiColorPicker((Rectangle){panelX + 45, 210, 140, 140}, "", &colorMalla);

    EndDrawing();
  }

  CloseWindow();
  return 0;
}
