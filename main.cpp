#include "parser.h" // Importamos nuestro nuevo archivo
#include "raylib.h"
#include "raymath.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#pragma GCC diagnostic ignored "-Wunused-parameter"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#pragma GCC diagnostic pop

int main() {
  const int screenWidth = 1280;
  const int screenHeight = 720;
  InitWindow(screenWidth, screenHeight, "Visor 3D Ligero - aLoonz");

  // --- SETUP DE FUENTE PERSONALIZADA ---
  // Si quieres usar una tipografía cyberpunk o de terminal, descomenta esto
  // y pon la ruta a tu archivo .ttf
  Font miFuente = LoadFontEx("JetBrainsMonoNerdFont-Light.ttf", 16, 0, 0);
  GuiSetFont(miFuente); // Aplica la fuente a los botones de RayGUI

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

  Color colorMalla = {50, 114, 172, 255};
  float escala = 0.01f;

  bool modoSolido = false;

  // Configuración visual básica para que el botón no desentone con tu fondo
  GuiSetStyle(DEFAULT, TEXT_SIZE, 14);

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

    // La interface va a partir de aqui
    DrawFPS(10, 10);

    // Texto descriptivo dinámico
    const char *textoModo =
        modoSolido ? "Modo Actual: Solido" : "Modo Actual: Alambrico";
    // Si cargaste una fuente personalizada arriba, usa esta línea en lugar de
    // DrawText:
    DrawTextEx(miFuente, textoModo, (Vector2){10, 35}, 14, 1,
               modoSolido ? LIGHTGRAY : colorMalla);
    // DrawText(textoModo, 10, 35, 14, modoSolido ? LIGHTGRAY : colorMalla);

    // GuiButton devuelve 'true' justo en el fotograma en que el usuario le hace
    // clic.
    // Botón movido a la esquina superior derecha:
    // Posición X = anchoPantalla(1280) - anchoBoton(150) - margen(20) = 1110
    if (GuiButton((Rectangle){1110, 10, 150, 25}, "Alternar Modo")) {
      modoSolido = !modoSolido;
    }

    EndDrawing();
  }

  CloseWindow();
  return 0;
}
