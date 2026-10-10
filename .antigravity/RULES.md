# REGLAS Y DIRECTRICES TÉCNICAS ESTRICTAS (STRICT RULES)

## Regla 1: Protocolo de Comunicación y Compatibilidad hacia Atrás
- NO sustituir el protocolo de comunicación actual por JSON en `algoritmo.cpp`.
- Mantener el patrón existente: extender el menú interactivo de `algoritmo.cpp` agregando nuevas opciones:
  - `8`: Algoritmo de Dijkstra (pide origen y destino).
  - `9`: Algoritmo de Bellman-Ford (pide origen y destino).
  - `10`: Benchmark comparativo (Dijkstra vs Bellman-Ford).
- En `interfaz.py`, registrar las opciones 8, 9 y 10 dentro de `_entrada_cpp` respetando el formato del guion de respuestas `stdin`.

## Regla 2: Implementación en C++ Puro (Cero Librerías Externas)
- Para Dijkstra y Bellman-Ford está ESTRICTAMENTE PROHIBIDO el uso de Boost.Graph, Lemon o cualquier biblioteca externa.
- Usar únicamente la STL básica de C++ (`<vector>`, `<queue>`, `<chrono>`, `<iostream>`, `<iomanip>`, `<string>`).
- En Dijkstra: usar `std::priority_queue` con pares `(distancia, vertice)`. Rechazar si alguna arista del grafo tiene peso $< 0$.
- En Bellman-Ford: relajar todas las aristas $\vert{}V\vert{}-1$ veces. Ejecutar la iteración $\vert{}V\vert{}$ para detectar ciclos de peso negativo y abortar con advertencia explícita.
- Medir los tiempos de cómputo en C++ con `std::chrono::high_resolution_clock` en microsegundos ($\mu s$).

## Regla 3: Motor Único en C++ (Cero Duplicidad de Código y Dependencia Estricta)
- El motor nativo en C++ (`algoritmo.exe`) es la ÚNICA fuente de verdad para todos los cálculos algorítmicos y de consultas (Dijkstra, Bellman-Ford, Benchmark, ciclos, caminos, adyacencias, etc.).
- Queda terminantemente PROHIBIDO el uso de motores de respaldo o funciones espejo en Python.
- Si `algoritmo.exe` no existe, no está compilado, supera el timeout o falla su ejecución, el sistema debe emitir un error explícito e inequívoco al usuario (mediante `messagebox.showerror` y en el área de texto de resultados con el mensaje formal de error), sin intentar calcular alternativas en Python.

## Regla 4: Renderizado Gráfico en Python sin Dependencias Adicionales
- NO instalar ni importar `networkx`.
- El resaltado del camino más corto (HU-12) debe implementarse extendiendo `_dibujar_en_eje` y `crear_figura_grafo` en `interfaz.py`:
  - Recibir un parámetro opcional `camino_resaltado: list[int] | None = None`.
  - Si un par $(u, v)$ pertenece al camino, dibujarlo con mayor `mutation_scale`, `linewidth=3.0` y color destacado (ej. `#e63946`).
  - Resaltar los nodos que pertenecen al camino con un color de relleno diferente.

## Regla 5: Corrección de Errores Existentes
- Blindar el `Spinbox` de cantidad de nodos contra valores alfanuméricos no numéricos (ej. capturar `TclError` y `ValueError` en `cantidad_var.get()`, asegurando el rango $2 \le n \le 26$).