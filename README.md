# Visor 3D Ligero

Visor de mallas poligonales 3D ligero y eficiente desarrollado en **C++** utilizando **Raylib** para el contexto gráfico y **Assimp** (Open Asset Import Library) para el procesamiento y carga de mallas multiformato, así como la carga de un material por defecto.

Este proyecto fue diseñado e implementado siguiendo el mapa curricular de la asignatura optativa **"Computación Gráfica/Computer Graphics"** de la **Escuela Superior de Cómputo (ESCOM - IPN)**.

---

## Características Principales

- **Soporte Multiformato:** Capacidad de importar mallas en formatos `.obj`, `.fbx` (mallas estáticas) y `.stl` de forma fluida a través de la infraestructura de Assimp.
- **Motor Matemático de Transformaciones Afines:** Control por medio de sliders para:
  - **Rotación:** Multiplicación matricial de vértices y vectores normales en los ejes $X$, $Y$ y $Z$.
  - **Escalamiento:** Escala adaptativa automática mediante *Bounding Box*.
- **Sombreado Lambert:**
  - Implementación en código del modelo de reflexión difusa pura.
  - Cálculo geométrico de vectores normales por cara mediante Producto Cruz de vectores directores.
  - Manipulación interactiva tridimensional del vector de dirección de la luz incidental.
- **Interfaz de Usuario (UX/UI):** Diseñada sobre un paradigma de modo inmediato (*Immediate-Mode GUI*) con `raygui.h`, adaptada a cambios de tamaño de ventana.
- **Efecto de Suelo Infinito:** Configuración optimizada de una rejilla espacial tridimensional extensa para evitar pérdidas de profundidad visual en planos lejanos.

---

## Implementación matemática

### 1. Cálculo de Vectores Normales (Producto Cruz)
Para cada triángulo definido por los vértices $V_1, V_2, V_3$, se determinan dos vectores directores coplanares:
$$A = V_2 - V_1$$
$$B = V_3 - V_1$$

El vector perpendicular a la superficie (normal) se obtiene mediante el Producto Cruz normalizado:
$$N = \\frac{A \\times B}{\\|A \\times B\\|}$$

### 2. Modelo de Reflexión Difusa de Lambert
La intensidad lumínica final de cada cara se calcula en base al coseno del ángulo entre la normal de la superficie y la dirección de la luz ($L$), utilizando el Producto Punto:
$$\\text{Intensidad} = \\max(0, N \\cdot L) \\cdot \\text{Factor de Difusión} + \\text{Luz Ambiental}$$

---

## Estructura de Archivos

Para distribuir la aplicación a entornos Linux (**Ubuntu / Arch Linux**), el paquete compilable comprimido (`.tar.gz` o `.zip`) debe mantener la siguiente estructura limpia de dependencias binarias:

```text
📂 Visor3D_Linux_Source/
├── 📄 main.cpp          # Archivo principal, inicialización gráfica e interfaz UI
├── 📄 parser.cpp        # Pipeline de importación y procesamiento geométrico con Assimp
├── 📄 parser.h          # Definición de estructuras matemáticas de datos (Cara, Modelo3D)
├── 📄 raygui.h          # Biblioteca de interfaz gráfica de modo inmediato
├── 📄 run_ubuntu.sh     # Script de resolución de dependencias y autoconstrucción para Ubuntu
├── 📄 run_arch.sh       # Script de resolución de dependencias y autoconstrucción para Arch Linux
├── 📄 README.md         # Descripción del proyecto
└── 📂 JetBrainsMono/    # Tipografía utilizada para el proyecto
    └── 📄 JetBrainsMonoNerdFont-Bold.ttf
```

---

## Instrucciones de Construcción y Ejecución

Se deben de dar perimsos de ejecución al archivo correspondiente en caso de que no los tengan al descargarse.

### 1. Ubuntu / Debian

```Bash
chmod +x run_ubuntu.sh
./run_ubuntu.sh
```

### 2. Arch Linux

```Bash
chmod +x run_arch.sh
./run_arch.sh
```

### 3. Windows

La distribución para Windows se entrega compilada de manera nativa mediante MSYS2 (UCRT64) en una carpeta portable auto-contenida (Visor3D_Windows.zip). No requiere instalación previa de librerías ni compiladores en el sistema del usuario. Basta con descomprimir la carpeta y ejecutar:

```Text
Visor3D.exe
```

---

## Muestras de funcionamiento

### 1. Inicio del programa

<img width="1282" height="752" alt="Inicialización del programa" src="https://github.com/user-attachments/assets/8233ea8a-7e8b-40dd-9c50-981049371ac5" />

### 2. Modo mallado

<img width="1282" height="752" alt="Modelo cargado en modo malla 'Archwing 09 Phong.fbx'" src="https://github.com/user-attachments/assets/1fe3f229-25b4-4009-98b9-138ebe127c3e" />

### 3. Modo sólido

<img width="1282" height="752" alt="Modelo cargado en modo sólido 'Archwing 09 Phong.fbx'" src="https://github.com/user-attachments/assets/5086f240-0d42-46b5-af0c-c643a1ea662d" />
