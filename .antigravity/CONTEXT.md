# CONTEXTO DEL SISTEMA: LABORATORIO DE TEORÍA DE GRAFOS (C++ + PYTHON)

## 1. Arquitectura Base del Repositorio
El sistema es una aplicación híbrida para Windows que integra:
- **`algoritmo.cpp`:** Motor de consola en C++17 sin librerías externas. Lee comandos y datos mediante un menú interactivo en `stdin` (opciones 0 a 7 existentes) e imprime en `stdout`.
- **`interfaz.py`:** Frontend en Tkinter con visualización integrada en Matplotlib (sin NetworkX). Administra la matriz, construye el guion de respuestas para `algoritmo.exe` mediante `_entrada_cpp(opcion, args)`, ejecuta el subproceso con `subprocess.run` y procesa el texto resultante. Depende EXCLUSIVAMENTE de `algoritmo.exe` para todo procesamiento algorítmico; no cuenta con motor de respaldo en Python y emite un error explícito si el binario no está disponible.
- **Visualización:** Usa cálculo trigonométrico en circunferencia unitaria (`calcular_posiciones`) y trazos curvos con `FancyArrowPatch`.

## 2. Estado de Implementación
- **Motor C++ (`algoritmo.cpp` / `algoritmo.exe`):**
  - Opciones 1 a 7: Matriz, Representación formal G=(V,E), Adyacentes y grado, Análisis de secuencias, Exportar DOT/JSON, Todos los caminos simples/ciclos por backtracking, y Detección de ciclos por DFS tricolor (blanco/gris/negro).
  - **HU-10:** Algoritmo de Dijkstra en C++ (Opción 8) con `std::priority_queue` y validación estricta de pesos no negativos ($w \ge 0$).
  - **HU-11:** Algoritmo de Bellman-Ford en C++ (Opción 9) con soporte de pesos negativos y detección de ciclos de costo negativo (|V| iteraciones).
  - **HU-13:** Módulo de benchmarking comparativo (Opción 10) con tiempos en $\mu s$ medidos con `std::chrono`, conteo de relajaciones y tabla resumen.
- **Frontend Python (`interfaz.py`):**
  - Matriz visual interactiva con simetría automática mediante `StringVar` compartida, validación de celdas ('x' y números), constructor de secuencias y panel de visualización en Matplotlib.
  - **HU-12:** Resaltado gráfico de la ruta mínima en la pestaña "Gráfica" (aristas en `#d90429`, mayor grosor y escala de flecha, nodos en `#ffb703`).
  - Despacho estricto a C++ sin fallback en Python; manejo explícito de errores si el binario C++ no está disponible o falla.