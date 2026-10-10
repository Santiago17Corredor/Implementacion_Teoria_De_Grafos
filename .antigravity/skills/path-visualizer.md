# Skill: Resaltado Visual de Rutas en Matplotlib
**Objetivo:** Implementar HU-12 en `interfaz.py`.

**Tareas:**
1. Modificar `_dibujar_en_eje` para aceptar `camino_resaltado=None` (lista de índices de nodos).
2. Construir el conjunto de aristas que componen la ruta: `{(camino[i], camino[i+1]) for i in range(len(camino)-1)}`.
3. Al iterar las aristas del grafo:
   - Si la arista pertenece al camino: usar `color="#d90429"`, `linewidth=3.2`, `mutation_scale=18`.
   - Si no pertenece: usar `color="#8d99ae"`, `linewidth=1.2`, `alpha=0.6`.
4. Al dibujar los círculos de los nodos:
   - Si el nodo está en el camino: `facecolor="#ffb703"`, borde oscuro.
   - Resto de nodos: color base original.
5. Al pulsar "Ruta Dijkstra" o "Ruta Bellman-Ford", calcular la ruta y automáticamente cambiar a la pestaña "Gráfica" mostrando el camino resaltado.