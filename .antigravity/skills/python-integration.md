# Skill: Integrador de Backend y GUI en Python
**Objetivo:** Conectar las opciones 8, 9 y 10 en `interfaz.py`.

**Tareas:**
1. **Extender `_entrada_cpp`:**
   - Añadir soporte para las opciones 8, 9 y 10 enviando los nombres de origen y destino seleccionados en los comboboxes de la GUI.
2. **Gestión Exclusiva con algoritmo.exe (Sin Respaldo en Python):**
   - Ejecutar directamente `algoritmo.exe` mediante `ejecutar_cpp` / `_ejecutar_cpp`.
   - Si el binario no existe o falla, presentar mensaje explícito de error (`MENSAJE_ERROR_CPP`) sin fallback en Python.
3. **Añadir Botones en la GUI (Panel "3. Consultas"):**
   - Insertar de forma alineada en la cuadrícula:
     - `[ Ruta Dijkstra ]`
     - `[ Ruta Bellman-Ford ]`
     - `[ Comparar Métodos (Benchmark) ]`
4. **Manejo de Resultados:**
   - Extraer la lista de nodos resultante de la salida para enviarla al visualizador gráfico.
   - Presentar el texto explicativo en el área de texto de la pestaña "Resultados".