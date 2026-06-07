#include "parser.h"
#include "raylib.h"
#include "raymath.h"
#include "rlgl.h" // Low level lib, inyeccion de texturas manuales
#include <cmath>
#include <string>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#pragma GCC diagnostic ignored "-Wunused-parameter"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#pragma GCC diagnostic pop

int main() {
  SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_WINDOW_ALWAYS_RUN |
                 FLAG_MSAA_4X_HINT);
  InitWindow(1280, 720, "Visor 3D Ligero - Roman Ochoa Oliva");

  Font miFuente =
      LoadFontEx("./JetBrainsMono/JetBrainsMonoNerdFont-Bold.ttf", 18, 0, 0);
  GuiSetFont(miFuente);

  Modelo3D modeloActual;
  bool modeloCargado = false;
  Texture2D texturaActual = {0};
  bool usarTextura = false;

  Camera3D camera = {};
  camera.position = (Vector3){15.0f, 15.0f, 15.0f};
  camera.target = (Vector3){0.0f, 2.0f, 0.0f};
  camera.up = (Vector3){0.0f, 1.0f, 0.0f};
  camera.fovy = 45.0f;
  camera.projection = CAMERA_PERSPECTIVE;

  // SetTargetFPS(144);

  // --- VARIABLES DE ESTADO PARA UI ---
  Color colorModelo = {50, 114, 172, 255};
  Vector3 rotacionManual = {0.0f, 0.0f, 0.0f};
  Vector3 traslacionManual = {0.0f, 0.0f, 0.0f};
  float escalaManual = 1.0f;
  float escalaBase = 1.0f;
  bool modoSolido = false;

  // --- CONFIGURACIÓN DE LUZ (Tema 2.4.2) ---
  Vector3 lightRawDir = {0.5f, 1.0f, 0.5f};

  GuiSetStyle(DEFAULT, TEXT_SIZE, 14);

  while (!WindowShouldClose()) {

    // --- DRAG & DROP ---
    if (IsFileDropped()) {
      FilePathList archivosSoltados = LoadDroppedFiles();
      if (archivosSoltados.count > 0) {
        std::string rutaArchivo = archivosSoltados.paths[0];

        // Si es Modelo 3D
        if (IsFileExtension(rutaArchivo.c_str(), ".obj") ||
            IsFileExtension(rutaArchivo.c_str(), ".fbx") ||
            IsFileExtension(rutaArchivo.c_str(), ".stl")) {
          modeloActual = cargar_modelo(rutaArchivo);
          if (modeloActual.vertices.size() > 0) {
            modeloCargado = true;
            rotacionManual = {0.0f, 0.0f, 0.0f};
            traslacionManual = {0.0f, 0.0f, 0.0f};
            escalaManual = 1.0f;

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

        // Si es Imagen 2D (Textura)
        else if (IsFileExtension(rutaArchivo.c_str(), ".png") ||
                 IsFileExtension(rutaArchivo.c_str(), ".jpg")) {
          if (texturaActual.id != 0)
            UnloadTexture(texturaActual);
          texturaActual = LoadTexture(rutaArchivo.c_str());
          usarTextura = true;
        }
      }
      UnloadDroppedFiles(archivosSoltados);
    }

    UpdateCamera(&camera, CAMERA_ORBITAL);

    if (IsKeyPressed(KEY_SPACE))
      modoSolido = !modoSolido;

    // PIPELINE MATRICIAL: Rotacion y Traslacion
    Matrix matRotacion = MatrixRotateXYZ((Vector3){rotacionManual.x * DEG2RAD,
                                                   rotacionManual.y * DEG2RAD,
                                                   rotacionManual.z * DEG2RAD});
    Matrix matTraslacion = MatrixTranslate(
        traslacionManual.x, traslacionManual.y, traslacionManual.z);

    float escalaFinal = escalaBase * escalaManual;
    Vector3 lightDir = Vector3Normalize(lightRawDir);

    BeginDrawing();
    ClearBackground(DARKGRAY);

    BeginMode3D(camera);

    if (modeloCargado) {
      for (const auto &cara : modeloActual.caras) {
        // Vértices originales.
        Vector3 v1 = modeloActual.vertices[cara.v1];
        Vector3 v2 = modeloActual.vertices[cara.v2];
        Vector3 v3 = modeloActual.vertices[cara.v3];

        // 1. Rotacion
        v1 = Vector3Transform(v1, matRotacion);
        v2 = Vector3Transform(v2, matRotacion);
        v3 = Vector3Transform(v3, matRotacion);

        // 2. Escala
        v1 = Vector3Scale(v1, escalaFinal);
        v2 = Vector3Scale(v2, escalaFinal);
        v3 = Vector3Scale(v3, escalaFinal);

        // 3. Traslacion
        v1 = Vector3Transform(v1, matTraslacion);
        v2 = Vector3Transform(v2, matTraslacion);
        v3 = Vector3Transform(v3, matTraslacion);

        if (modoSolido) {
          Vector3 normalRotada = Vector3Transform(cara.normal, matRotacion);

          float dot = Vector3DotProduct(normalRotada, lightDir);
          if (dot < 0.0f)
            dot = 0.0f; // Evitamos luz negativa

          // Intensidad = (Difusa * dot) + Ambiental (0.3 para no ver negro
          // total)
          float intensidad = (dot * 0.7f) + 0.3f;

          Color colorFinal = {(unsigned char)(colorModelo.r * intensidad),
                              (unsigned char)(colorModelo.g * intensidad),
                              (unsigned char)(colorModelo.b * intensidad),
                              colorModelo.a};

          if (usarTextura && texturaActual.id != 0) {
            rlSetTexture(texturaActual.id);
            rlBegin(RL_TRIANGLES);
            rlColor4ub(colorFinal.r, colorFinal.g, colorFinal.b, colorFinal.a);

            rlTexCoord2f(cara.uv1.x, cara.uv1.y);
            rlVertex3f(v1.x, v1.y, v1.z);
            rlTexCoord2f(cara.uv2.x, cara.uv2.y);
            rlVertex3f(v2.x, v2.y, v2.z);
            rlTexCoord2f(cara.uv3.x, cara.uv3.y);
            rlVertex3f(v3.x, v3.y, v3.z);

            rlEnd();
            rlSetTexture(0);
          } else {
            DrawTriangle3D(v1, v2, v3, colorFinal);
          }

          // Líneas de contorno sutiles para resaltar la geometría
          DrawLine3D(v1, v2, (Color){30, 30, 30, 100});
          DrawLine3D(v2, v3, (Color){30, 30, 30, 100});
          DrawLine3D(v3, v1, (Color){30, 30, 30, 100});
        } else {
          DrawLine3D(v1, v2, colorModelo);
          DrawLine3D(v2, v3, colorModelo);
          DrawLine3D(v3, v1, colorModelo);
        }
      }
    }

    DrawGrid(400, 10.0f);
    EndMode3D();

    // --- PANEL DE CONTROL LATERAL ---
    int w = GetScreenWidth();
    int h = GetScreenHeight();

    DrawFPS(10, 10);
    DrawText(modoSolido ? "Modo: Solido" : "Modo: Malla", 10, 35, 14,
             LIGHTGRAY);

    // Mensaje centrado si no hay modelo, usando las dimensiones dinámicas
    if (!modeloCargado) {
      const char *msg = "Arrastra un archivo 3D (.obj, .fbx, .stl) o una "
                        "imagen (.png, .jpg) aquí";
      DrawText(msg, (w - MeasureText(msg, 20)) / 2, h / 2, 20, LIGHTGRAY);
    }

    float panelX = w - 240.0f;
    GuiPanel((Rectangle){panelX, 10, 230, 620}, "Herramientas");

    if (GuiButton((Rectangle){panelX + 15, 35, 200, 25}, "Alternar Modo")) {
      modoSolido = !modoSolido;
    }

    // --- ROTACION ---
    GuiLabel((Rectangle){panelX + 15, 65, 100, 20}, "Rotacion X");
    GuiSliderBar((Rectangle){panelX + 90, 65, 120, 20}, "",
                 TextFormat("%.0f", rotacionManual.x), &rotacionManual.x, 0.0f,
                 360.0f);

    GuiLabel((Rectangle){panelX + 15, 90, 100, 20}, "Rotacion Y");
    GuiSliderBar((Rectangle){panelX + 90, 90, 120, 20}, "",
                 TextFormat("%.0f", rotacionManual.y), &rotacionManual.y, 0.0f,
                 360.0f);

    GuiLabel((Rectangle){panelX + 15, 115, 100, 20}, "Rotacion Z");
    GuiSliderBar((Rectangle){panelX + 90, 115, 120, 20}, "",
                 TextFormat("%.0f", rotacionManual.z), &rotacionManual.z, 0.0f,
                 360.0f);

    // --- TRASLACION ---
    GuiLabel((Rectangle){panelX + 15, 145, 100, 20}, "Traslacion X");
    GuiSliderBar((Rectangle){panelX + 90, 145, 120, 20}, "",
                 TextFormat("%.1f", traslacionManual.x), &traslacionManual.x,
                 -20.0f, 20.0f);
    GuiLabel((Rectangle){panelX + 15, 170, 100, 20}, "Traslacion Y");
    GuiSliderBar((Rectangle){panelX + 90, 170, 120, 20}, "",
                 TextFormat("%.1f", traslacionManual.y), &traslacionManual.y,
                 -20.0f, 20.0f);
    GuiLabel((Rectangle){panelX + 15, 195, 100, 20}, "Traslacion Z");
    GuiSliderBar((Rectangle){panelX + 90, 195, 120, 20}, "",
                 TextFormat("%.1f", traslacionManual.z), &traslacionManual.z,
                 -20.0f, 20.0f);

    // --- ESCALA ---
    GuiLabel((Rectangle){panelX + 15, 225, 100, 20}, "Escala");
    GuiSliderBar((Rectangle){panelX + 90, 225, 120, 20}, "",
                 TextFormat("%.2fx", escalaManual), &escalaManual, 0.1f, 3.0f);

    // --- ILUMINACION DINÁMICA ---
    GuiLabel((Rectangle){panelX + 15, 260, 100, 20}, "Luz Vector X");
    GuiSliderBar((Rectangle){panelX + 90, 260, 120, 20}, "",
                 TextFormat("%.2f", lightRawDir.x), &lightRawDir.x, 0.0f, 1.0f);

    GuiLabel((Rectangle){panelX + 15, 285, 100, 20}, "Luz Vector Y");
    GuiSliderBar((Rectangle){panelX + 90, 285, 120, 20}, "",
                 TextFormat("%.2f", lightRawDir.y), &lightRawDir.y, -1.0f,
                 1.0f);

    GuiLabel((Rectangle){panelX + 15, 310, 100, 20}, "Luz Vector Z");
    GuiSliderBar((Rectangle){panelX + 90, 310, 120, 20}, "",
                 TextFormat("%.2f", lightRawDir.z), &lightRawDir.z, -1.0f,
                 1.0f);

    // --- COLOR ---
    GuiLabel((Rectangle){panelX + 15, 340, 100, 20}, "Color / Tinte:");
    GuiColorPicker((Rectangle){panelX + 45, 365, 140, 140}, "", &colorModelo);
    GuiCheckBox((Rectangle){panelX + 15, 520, 20, 20}, "Activar Textura 2D",
                &usarTextura);

    EndDrawing();
  }

  if (texturaActual.id != 0)
    UnloadTexture(texturaActual);
  CloseWindow();
  return 0;
}
