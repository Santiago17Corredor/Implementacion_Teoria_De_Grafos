# Plan incremental según las historias de usuario

Este documento actualiza el plan a partir de `Historias de usuario.md`, revisado el 7 de octubre de 2026. La implementación se entrega por partes: una parte terminada y revisable por turno para poder hacer un commit y un push con calma.

## Forma de programar y entregar

- C++ procedural, funciones descriptivas en español y biblioteca estándar.
- Código entendible, sin clases ni abstracciones innecesarias.
- Comentarios breves para explicar decisiones que no sean evidentes.
- Una entrega incluye el cambio funcional, su explicación y pruebas acordes al cambio.
- Los commits y el push los hace el usuario. No se modificará el respaldo `algoritmo_previo.cpp`.
- Primero se completan los requisitos generales del grafo (HU-01 a HU-09). Los algoritmos del Lab 4 quedan en entregas posteriores.
- No se considera aprobada una prueba que no se haya ejecutado.

## Secuencia de entregas

| Parte | Historias | Cambio verificable | Estado |
| --- | --- | --- | --- |
| 1 | HU-02 y adaptación de sus consumidores | Mínimo de dos nodos; identificadores personalizados y únicos; consultas, matriz y DOT compatibles. | Implementada; prueba de ejecución pendiente de compilador |
| 2 | HU-04 | Validar todas las dimensiones antes de acceder a celdas; mensajes de inconsistencias; resolver captura y tratamiento de lazos. | Pendiente |
| 3 | HU-01, HU-03, HU-05 | Selección ponderado/no ponderado; captura de pesos y ausencia de conexión; representación matemática completa. | Pendiente |
| 4 | HU-07 | Mostrar grados, grados de entrada/salida y aviso explícito de nodo aislado. | Pendiente |
| 5 | HU-08 | Buscar y enumerar caminos simples entre origen y destino; secuencia, longitud y costo; tratar origen igual a destino. | Pendiente |
| 6 | HU-09 | Detectar automáticamente ciclos en todo el grafo y mostrar al menos uno; cubrir componentes desconectadas. | Pendiente |
| 7 | Arquitectura híbrida, base para HU-06 | Separar entrada/salida de la lógica; definir comunicación C++/Python para transferir el grafo y errores. | Pendiente |
| 8 | HU-06 | Dibujar desde Python nombres, flechas y pesos; comprobar nodos aislados y legibilidad. | Pendiente |
| 9 | HU-10 | Dijkstra propio en C++, pesos no negativos, reconstrucción de ruta y destino inalcanzable. | Posterior: Lab 4 |
| 10 | HU-11 | Bellman-Ford propio, iteraciones requeridas, ruta y detección de ciclos negativos alcanzables. | Posterior: Lab 4 |
| 11 | HU-12 y arquitectura híbrida | Completar `subprocess`, JSON, timeout y códigos de retorno; resaltar rutas y presentar costo/algoritmo en Python. | Posterior: Lab 4 |
| 12 | HU-13 | Medir tiempos y operaciones en C++; comparar costos válidos y presentar las métricas en Python. | Posterior: Lab 4 |

Cada parte se revisa antes de continuar. El estado de una historia con varios criterios sigue siendo parcial hasta completar todos ellos.

## Decisiones vigentes en la parte 1

- Cantidad permitida: **2 a 26 nodos**. Se conserva el máximo acordado para esta consola; las historias no fijan un máximo nuevo.
- Los nombres se guardan con `vector<string>` y se ingresan **uno por línea**. Una coma puede formar parte del nombre; no es un separador en esta modalidad.
- Se permiten letras, números, palabras, espacios internos y símbolos imprimibles.
- Se quitan espacios y tabulaciones en los extremos. Un nombre vacío o con caracteres de control internos se rechaza.
- Se conserva la escritura: `A` y `a` son nombres distintos. La consulta debe usar el nombre registrado.
- La matriz adapta su ancho al nombre más largo. La apariencia de caracteres Unicode depende de la codificación y la fuente de la terminal; `setw` mide bytes, no el ancho visual de todos los símbolos.
- La representación matemática escribe nombres entre comillas para distinguir identificadores que contienen signos de puntuación.
- DOT identifica los nodos internamente como `n0`, `n1`, etc. Los nombres se guardan como etiquetas con comillas y barras invertidas escapadas.
- Si se acaba la entrada, el programa informa el cierre y termina con código 1; no repite preguntas indefinidamente.
- Las conexiones siguen siendo binarias en esta entrega. Los pesos se implementarán en la parte 3.

## Decisiones por cerrar antes de las partes correspondientes

1. **Lazos (parte 2):** HU-04 pide detectarlos e informarlos; falta acordar si se conservarán o se rechazarán después del aviso. Actualmente la diagonal se mantiene en cero.
2. **Pesos (parte 3):** definir si se aceptan decimales y su rango. La ausencia de arista debe distinguirse del peso cero; se propone `x` al capturar, con una representación interna separada.
3. **Enumeración (parte 5):** se propone enumerar caminos simples, porque permitir vueltas repetidas en ciclos puede producir infinitos recorridos. No se truncarán resultados silenciosamente; cualquier límite debe indicarse.
4. **Comparación (parte 12):** proponer al equipo validar igual costo mínimo y rutas válidas. Dos algoritmos pueden devolver rutas distintas con el mismo costo. Exigir una ruta idéntica requiere acordar el desempate.

## Parte 1: contenido y verificación

Archivos de la entrega:

- `algoritmo.cpp`: implementación.
- `EXPLICACION_LAB3.md`: explicación actualizada.
- `PLAN_LAB3.md`: referencia al plan incremental y ajustes de la primera etapa.
- `PLAN_INCREMENTAL.md`: seguimiento de las próximas entregas.
- `pruebas/probar_hu02.py`: comprobaciones del ejecutable, usando solamente la biblioteca estándar de Python.

Compilar únicamente el programa actual, sin incluir el archivo de respaldo:

```powershell
g++ -std=c++17 -Wall -Wextra -pedantic algoritmo.cpp -o algoritmo.exe
python pruebas/probar_hu02.py .\algoritmo.exe
```

Las pruebas usan un directorio temporal y no reemplazan el `grafo.dot` del usuario. Cubren mínimo/máximo, entradas inválidas, duplicados, mayúsculas, nombres con espacios y símbolos, consultas, caminos, ciclos, DOT y cierre por fin de entrada.

Si no se dispone de compilador, ejecutar estos casos manualmente cuando esté disponible:

| Caso | Entrada o acción | Resultado esperado |
| --- | --- | --- |
| Mínimo | Escribir `1` y después `2`. | Aviso específico de mínimo dos nodos; vuelve a pedir cantidad. |
| Tipos de nombres | Crear `v1`, `15`, `@` y `Nodo central`. | Los cuatro identificadores se conservan en todas las operaciones. |
| Duplicado | Crear `v1` y luego ` v1 `. | Se rechaza el segundo; permite renombrarlo. |
| Vacío | Introducir una línea vacía o solo espacios. | Pide un nombre válido. |
| Mayúsculas | Crear `A` y `a`. | Son dos nodos diferentes y se consultan con su escritura exacta. |
| Dirección | Crear solo `origen -> destino`. | El recorrido inverso no es camino. |
| DOT | Usar comillas y barras invertidas en nombres. | Se conservan como etiquetas escapadas; no se usan como identificadores DOT. |
| Fin de entrada | Cerrar stdin al pedir un dato. | Termina con aviso; no entra en un ciclo de preguntas. |

Verificación de esta entrega: `git diff --check` sin errores; referencias a los nombres de un carácter reemplazadas; sintaxis y ayuda del script de pruebas comprobadas con Python. La compilación y las pruebas sobre el ejecutable quedan pendientes porque no se encontró un compilador disponible en Windows ni una distribución WSL instalada.

## Commit sugerido para la parte 1

Revisar los cambios y agregar solo esta entrega:

```powershell
git diff --check
git diff --stat
git add algoritmo.cpp PLAN_LAB3.md PLAN_INCREMENTAL.md EXPLICACION_LAB3.md pruebas/probar_hu02.py
git diff --cached --stat
git commit -m "Implementar HU-02: vertices personalizados y minimo de dos nodos"
git push
```

Estos comandos son una guía; no se han ejecutado el `git add`, el commit ni el push. Revisar también los archivos que ya pudieran estar preparados antes del commit.
