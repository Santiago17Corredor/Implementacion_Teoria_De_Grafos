# Skill: Implementador de Algoritmos de Camino Mínimo en C++
**Objetivo:** Extender `algoritmo.cpp` con las opciones 8, 9 y 10.

**Tareas:**
1. **Opción 8 - Dijkstra:**
   - Pedir nodo origen y destino (con `leerIndiceNodo`).
   - Validar que ninguna arista del grafo tenga peso $< 0$. Si hay aristas negativas, imprimir error y volver al menú.
   - Ejecutar Dijkstra desde cero: `vector<double> dist(n, INF)`, `vector<int> prev(n, -1)`, `priority_queue`.
   - Contabilizar operaciones de relajación (`relajaciones++`).
   - Cronometrar con `std::chrono::high_resolution_clock`.
   - Reconstruir e imprimir la ruta: `A -> B -> C`, el costo total, el tiempo en µs y las relajaciones.
2. **Opción 9 - Bellman-Ford:**
   - Pedir nodo origen y destino.
   - Aceptar aristas con pesos negativos.
   - Ejecutar $\vert{}V\vert{}-1$ iteraciones sobre todas las aristas existentes.
   - Ejecutar la iteración $\vert{}V\vert{}$: si alguna arista se puede relajar, imprimir: `ALERTA: Se detectó un ciclo de costo negativo alcanzable.`
   - Si no hay ciclo negativo, mostrar la ruta, costo, tiempo en µs y relajaciones.
3. **Opción 10 - Benchmark Comparativo:**
   - Ejecutar Dijkstra y Bellman-Ford sobre el mismo par origen-destino.
   - Comparar consistencia de costos e imprimir tabla formateada con columnas: Métrica, Dijkstra, Bellman-Ford.