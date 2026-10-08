# Plan incremental según las historias de usuario

Este documento actualiza el plan a partir de `Historias de usuario.md`, revisado el 7 de octubre de 2026. La implementación se entrega por bloques revisables para poder hacer un commit y un push con calma. Por petición del usuario de aprovechar la ventana restante, las partes 3 y 4 se agruparon en una entrega; no se avanzó al Lab 4.

## Forma de programar y entregar

- C++ procedural, funciones descriptivas en español y biblioteca estándar.
- Código entendible, sin clases con lógica ni abstracciones innecesarias. `Conexion` es un registro de dos datos, sin métodos; las operaciones siguen en funciones independientes.
- Comentarios breves para explicar decisiones que no sean evidentes.
- Una entrega incluye el cambio funcional, su explicación y pruebas acordes al cambio.
- Los commits y el push los hace el usuario. No se modificará el respaldo `algoritmo_previo.cpp`.
- Primero se completan los requisitos generales del grafo (HU-01 a HU-09). Los algoritmos del Lab 4 quedan en entregas posteriores.
- No se considera aprobada una prueba que no se haya ejecutado.

## Secuencia de entregas

| Parte | Historias | Cambio verificable | Estado |
| --- | --- | --- | --- |
| 1 | HU-02 y adaptación de sus consumidores | Mínimo de dos nodos; identificadores personalizados y únicos; consultas, matriz y DOT compatibles. | Implementada; prueba de ejecución pendiente de compilador |
| 2 | HU-04 | Validar todas las dimensiones antes de acceder a celdas; mensajes de inconsistencias; detectar lazos al capturar y pedir corregirlos. | Implementada y adaptada a pesos en parte 3; ejecución pendiente de compilador |
| 3 | HU-01, HU-03, HU-05 | Selección ponderado/no ponderado; captura de pesos y ausencia de conexión; representación matemática completa. | Implementada en consola y DOT; renderizado de HU-01 pendiente con HU-06; ejecución pendiente de compilador |
| 4 | HU-07 | Mostrar grados, grados de entrada/salida y aviso explícito de nodo aislado. | Implementada; ejecución pendiente de compilador |
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

1. **Lazos (resuelto en partes 2 y 3):** se conserva la regla sin lazos. En modo binario se informa un `1` diagonal y se pide `0`. En modo ponderado, cualquier peso diagonal, incluido cero, se informa como lazo y se pide `x`.
2. **Pesos (resuelto en parte 3):** `double` finito entre ±1000000000; acepta decimales con punto y notación científica. `x`/`X` representa ausencia; cero es un peso válido. Un registro `Conexion` mantiene separados existencia y costo. Es una decisión práctica de captura, no un límite impuesto por las historias.
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

## Parte 2: integridad de la matriz (HU-04)

Cambios implementados:

- Validar rango de nodos, cantidad de nombres y tipo de grafo.
- Revisar la longitud de todas las filas antes de acceder a cualquier celda, incluida la diagonal y la posición simétrica.
- Rechazar valores distintos de `0/1`, identificando la celda y el valor recibido.
- Informar el nodo de un lazo y conservar la restricción de grafos simples.
- Informar las dos direcciones y sus valores cuando una matriz no dirigida sea asimétrica.
- Devolver el detalle mediante `string& error`; la función de validación no imprime ni modifica los datos.
- Permitir corregir errores de captura y volver a ingresar la matriz si la revisión final encuentra una inconsistencia.

Nuevo orden de captura: todas las celdas para grafos dirigidos; triángulo superior incluida la diagonal para no dirigidos. Ejemplo de tres nodos no dirigidos: `A-A`, `A-B`, `A-C`, `B-B`, `B-C`, `C-C`. La captura refleja cada valor automáticamente, por lo que no se generan asimetrías desde esta modalidad de consola.

La historia se cubre para la matriz binaria actual, con la decisión de rechazar lazos después de informarlos. En la parte 3 se adaptarán estas validaciones a los pesos; no se considera terminado el soporte ponderado.

### Pruebas de la parte 2

```powershell
g++ -std=c++17 -Wall -Wextra -pedantic algoritmo.cpp -o algoritmo.exe
python pruebas/probar_hu02.py .\algoritmo.exe
python pruebas/probar_hu04.py .\algoritmo.exe
g++ -std=c++17 -Wall -Wextra -pedantic -D_GLIBCXX_ASSERTIONS pruebas/validar_matriz.cpp -o prueba_matriz.exe
.\prueba_matriz.exe
```

- HU-02 conserva sus casos, adaptados a las nuevas preguntas de la diagonal.
- HU-04 prueba captura, corrección de lazos, simetría automática, dirección y cierre de entrada durante una corrección.
- Las pruebas C++ llaman al validador real con 16 casos: matrices vacías, tamaños inválidos, nombres insuficientes, filas vacías/cortas/largas, valores fuera de rango, lazos, asimetría y ejemplos válidos. También comprueban que el mensaje de un error anterior se limpie al validar correctamente.
- La unidad C++ se compila por separado porque incluye el programa con su `main` renombrado dentro de esa prueba. No compilar todos los `.cpp` juntos.

Verificación: revisión estática y de diferencias; sintaxis de los scripts Python comprobada. Las pruebas del ejecutable y las 16 pruebas C++ quedan pendientes de compilador; no se reportan como aprobadas.

### Commit sugerido para la parte 2

```powershell
git diff --check
git add algoritmo.cpp PLAN_LAB3.md PLAN_INCREMENTAL.md EXPLICACION_LAB3.md pruebas/probar_hu02.py pruebas/probar_hu04.py pruebas/validar_matriz.cpp
git diff --cached --stat
git commit -m "Implementar HU-04: validar matriz y detectar lazos"
git push
```

Las partes 3 y 4 se implementaron en la entrega siguiente, descrita abajo. Las instrucciones anteriores se conservan como historial de cada bloque.

## Partes 3 y 4: pesos, grados y aislamiento

### Alcance implementado

- Elegir dirección y ponderación de forma independiente, manteniendo modo binario `0/1`.
- Capturar pesos positivos, negativos y cero, con `x` para ausencia.
- Usar `Conexion { existe, peso }` como celda. Sigue siendo programación procedural, sin una clase `Grafo` ni dependencias nuevas.
- Validar dimensiones antes de consultar las celdas, indicadores binarios, pesos finitos dentro del rango, coherencia de la ausencia y simetría tanto de existencia como de peso.
- Rechazar lazos ponderados incluso cuando su costo es cero; permitir corregir solo el dato incorrecto.
- Mostrar matriz con nombres y ancho ajustado también a los pesos, conjuntos formales con tuplas ponderadas y etiquetas de pesos en DOT.
- Conservar consultas y verificación manual de secuencias para los cuatro tipos. Las conexiones de costo cero cuentan como aristas; se informa costo acumulado en secuencias ponderadas válidas.
- Contar grado no dirigido y grados de entrada/salida en dirigido. Un nodo dirigido es aislado solo si ambos grados son cero. El grado cuenta conexiones, no suma sus pesos.

Los límites actuales son 2–26 nodos, pesos entre ±mil millones y secuencias manuales de 1–100 nodos. La suma de una secuencia no desborda con estos límites; `double` sigue teniendo redondeos y se presentan hasta 15 cifras significativas. No se añadieron búsquedas de caminos, detección automática de ciclos, interfaz Python ni rutas mínimas.

HU-03 usa la opción binaria permitida por la historia; no se aceptan `T/F`. HU-01 aún depende del renderizado de HU-06 para su criterio gráfico. Exportar DOT no equivale a dibujar con Python. Ninguna historia se considera verificada en ejecución hasta correr las pruebas C++.

### Comprobaciones y pruebas

```powershell
g++ -std=c++17 -Wall -Wextra -pedantic algoritmo.cpp -o algoritmo.exe
python pruebas/probar_hu02.py .\algoritmo.exe
python pruebas/probar_hu04.py .\algoritmo.exe
python pruebas/probar_ponderados.py .\algoritmo.exe
python pruebas/probar_hu07.py .\algoritmo.exe
g++ -std=c++17 -Wall -Wextra -pedantic -D_GLIBCXX_ASSERTIONS pruebas/validar_matriz.cpp -o prueba_matriz.exe
.\prueba_matriz.exe
g++ -std=c++17 -Wall -Wextra -pedantic -D_GLIBCXX_ASSERTIONS pruebas/validar_pesos.cpp -o prueba_pesos.exe
.\prueba_pesos.exe
```

- Las pruebas previas se adaptaron a la nueva pregunta de ponderación; se conservan los 16 casos de integridad binaria.
- Las pruebas nuevas de consola cubren cero frente a ausencia, negativos y decimales, tuplas, matriz alineada, DOT exacto, sentido de las aristas, costos, lazos, entradas inválidas, límites y fin de entrada.
- Las pruebas de HU-07 cubren los cuatro tipos de grafo, nodos aislados, nodos con solo entradas o solo salidas, nombres inexistentes y grados independientes del peso.
- Las pruebas directas de pesos cubren conversión, valores no finitos, desbordamiento y subdesbordamiento de conversión, simetría, integridad interna y caminos de costo cero.
- Se debe compilar cada archivo C++ por separado. No incluir `algoritmo_previo.cpp`.

Verificación local: revisión de diferencias y coherencia de consumidores, sintaxis y ayuda de los cuatro scripts Python. La compilación y ejecución contra el programa real siguen pendientes: no se encontró `g++`, `clang++` ni `cl` disponible. No se instalaron herramientas ni se reportan pruebas C++ como aprobadas.

### Commit sugerido para esta entrega

```powershell
git diff --check
git add algoritmo.cpp PLAN_LAB3.md PLAN_INCREMENTAL.md EXPLICACION_LAB3.md pruebas/probar_hu02.py pruebas/probar_hu04.py pruebas/validar_matriz.cpp pruebas/probar_ponderados.py pruebas/validar_pesos.cpp pruebas/probar_hu07.py
git diff --cached --stat
git commit -m "Agregar grafos ponderados y completar consulta de grados"
git push
```

No se ejecutaron `git add`, commit ni push. El siguiente bloque es **parte 5: búsqueda y enumeración de caminos simples**, sin algoritmos de ruta mínima.
