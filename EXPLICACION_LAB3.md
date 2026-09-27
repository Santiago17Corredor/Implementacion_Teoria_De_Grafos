# Explicación completa - Laboratorio 3: Teoría de grafos

## 1. ¿Qué se desarrolló?

Se desarrolló un programa de consola en C++ que permite crear y analizar un grafo simple mediante una matriz de adyacencia.

El programa permite:

- Definir entre 1 y 26 nodos.
- Asignar una letra diferente a cada nodo.
- Elegir entre un grafo dirigido y uno no dirigido.
- Ingresar las conexiones del grafo.
- Construir y mostrar su matriz de adyacencia.
- Mostrar la representación matemática del grafo.
- Consultar los nodos adyacentes.
- Comprobar si una secuencia de nodos es un camino.
- Comprobar si una secuencia forma un ciclo.
- Generar un archivo DOT para representar gráficamente el grafo con Graphviz.

El programa implementa solamente la Guía 3. No incluye rutas más cortas, Dijkstra, Bellman-Ford ni requisitos de la Guía 4.

## 2. Decisiones de diseño

### Programación procedural

El programa utiliza programación procedural: el problema se divide en funciones pequeñas, y cada función realiza una tarea específica.

Este enfoque se eligió porque:

- El programa administra un solo grafo durante cada ejecución.
- Las operaciones son pequeñas y fáciles de separar.
- Evita introducir clases que todavía no aportan una ventaja importante.
- Facilita explicar el código durante una sustentación.
- Mantiene el programa básico y entendible.

Utilizar POO también sería válido, pero no es necesario para cumplir el objetivo de esta práctica.

### Grafo simple y no ponderado

El programa trabaja con grafos simples y no ponderados:

- `0` significa que no existe una conexión.
- `1` significa que existe una conexión.
- No se permiten lazos de un nodo consigo mismo.
- La diagonal principal de la matriz permanece en cero.
- Las aristas no tienen costos ni pesos.

Los pesos no se incluyen porque pertenecen al trabajo posterior de rutas más cortas.

### Límite de 26 nodos

Cada nodo se identifica mediante una letra entre `A` y `Z`. Por esa razón, el límite máximo es de 26 nodos.

Este límite también es razonable para una aplicación de consola. Una matriz de 26 nodos contiene 676 posiciones y una gráfica de ese tamaño puede ser difícil de leer.

## 3. Estructuras de datos principales

### Vector de nombres

```cpp
vector<char> nombresNodos;
```

Guarda la letra asignada a cada nodo.

Por ejemplo:

```text
Índice:  0  1  2  3
Nodo:    A  B  C  D
```

El índice permite relacionar cada letra con una fila y una columna de la matriz.

### Matriz de adyacencia

```cpp
vector<vector<int>> matrizAdyacencia;
```

La matriz es cuadrada. Tiene el mismo número de filas y columnas que de nodos.

Si `matriz[i][j]` contiene `1`, existe una conexión desde el nodo `i` hacia el nodo `j`.

Si contiene `0`, no existe esa conexión.

Ejemplo:

```text
      A  B  C
  A   0  1  1
  B   1  0  0
  C   1  0  0
```

Este ejemplo representa las aristas `A-B` y `A-C`.

### Tipo de grafo

```cpp
enum class TipoGrafo {
    NoDirigido = 1,
    Dirigido = 2
};
```

El `enum class` evita trabajar por todo el programa con números sin significado evidente.

Es más claro escribir:

```cpp
tipo == TipoGrafo::Dirigido
```

que escribir:

```cpp
tipo == 2
```

## 4. Flujo general del programa

El programa sigue este orden:

```text
Inicio
  |
  v
Pedir cantidad de nodos
  |
  v
Pedir nombres únicos
  |
  v
Elegir tipo de grafo
  |
  v
Ingresar conexiones
  |
  v
Validar matriz
  |
  v
Mostrar resumen y matriz
  |
  v
Ejecutar menú de operaciones
  |
  v
Salir
```

La matriz se crea una sola vez. Después, el menú permite realizar varias consultas sobre el mismo grafo sin volver a ingresarlo.

## 5. Explicación de las funciones

### `leerEnteroEnRango`

```cpp
int leerEnteroEnRango(const string& mensaje, int minimo, int maximo);
```

Solicita un número entero y comprueba que esté dentro de un rango.

La entrada se recibe primero como una línea de texto y luego se analiza con `stringstream`. Esto permite rechazar correctamente casos como:

```text
hola
3abc
1 2
-5
100
```

El programa continúa preguntando hasta recibir un único entero válido.

Esta función se reutiliza para:

- Cantidad de nodos.
- Tipo de grafo.
- Valores de la matriz.
- Opciones del menú.
- Longitud de una secuencia.

### `convertirAMayuscula`

```cpp
char convertirAMayuscula(char letra);
```

Convierte una letra minúscula en mayúscula. Gracias a esto, `a` y `A` se interpretan como el mismo nodo.

### `esLetraValida`

```cpp
bool esLetraValida(char letra);
```

Comprueba que el carácter esté entre `A` y `Z`.

Rechaza números, símbolos y otros caracteres.

### `letraRepetida`

```cpp
bool letraRepetida(const vector<char>& nombres, char letra);
```

Recorre los nombres que ya fueron registrados y determina si la letra está repetida.

No se permiten dos nodos con el mismo nombre porque después no sería posible distinguirlos al consultar la matriz.

### `pedirLetra`

```cpp
char pedirLetra(const string& mensaje);
```

Solicita exactamente una letra, la convierte a mayúscula y valida que esté entre `A` y `Z`.

Ejemplos:

```text
a     -> válido; se convierte en A
AB    -> inválido
7     -> inválido
A B   -> inválido
```

### `pedirCantidadNodos`

```cpp
int pedirCantidadNodos();
```

Utiliza `leerEnteroEnRango` para aceptar cantidades entre 1 y 26.

### `pedirNombresNodos`

```cpp
void pedirNombresNodos(vector<char>& nombres, int cantidad);
```

Solicita el nombre de cada nodo y evita letras repetidas.

El vector se pasa por referencia porque la función debe modificarlo y agregar los nombres ingresados.

### `pedirTipoGrafo`

```cpp
TipoGrafo pedirTipoGrafo();
```

Presenta dos opciones:

```text
1. No dirigido
2. Dirigido
```

Luego transforma la opción numérica en un valor de `TipoGrafo`.

### `pedirValorAdyacencia`

```cpp
int pedirValorAdyacencia(char origen, char destino, TipoGrafo tipo);
```

Pregunta si existe una conexión entre dos nodos.

En un grafo no dirigido se muestra:

```text
Existe la arista A -- B? (0/1):
```

En un grafo dirigido se muestra:

```text
Existe la arista A -> B? (0/1):
```

Solamente acepta `0` o `1`.

### `ingresarMatriz`

```cpp
void ingresarMatriz(
    vector<vector<int>>& matriz,
    const vector<char>& nombres,
    TipoGrafo tipo
);
```

Construye la matriz de adyacencia a partir de las conexiones indicadas por el usuario.

#### Grafo dirigido

En un grafo dirigido, `A -> B` no significa lo mismo que `B -> A`. Por lo tanto, se pregunta por cada dirección de manera independiente.

```text
Existe A -> B
Existe B -> A
```

Las dos respuestas pueden ser diferentes.

#### Grafo no dirigido

En un grafo no dirigido, `A -- B` es la misma arista que `B -- A`.

El programa pregunta una sola vez y copia el resultado en las dos posiciones:

```cpp
matriz[i][j] = valor;
matriz[j][i] = valor;
```

Así se garantiza automáticamente que la matriz sea simétrica.

La diagonal no se pregunta porque los lazos no están permitidos y sus valores permanecen en cero.

### `validarMatriz`

```cpp
bool validarMatriz(
    const vector<vector<int>>& matriz,
    TipoGrafo tipo
);
```

Comprueba que:

- La matriz no esté vacía.
- Sea cuadrada.
- Todos los valores sean `0` o `1`.
- La diagonal principal esté en cero.
- Sea simétrica si el grafo es no dirigido.

Aunque la entrada controlada ya evita muchos errores, esta segunda validación protege la integridad del grafo antes de utilizarlo.

### `mostrarResumen`

```cpp
void mostrarResumen(
    const vector<char>& nombres,
    TipoGrafo tipo
);
```

Muestra la cantidad de nodos y si el grafo es dirigido o no dirigido.

### `imprimirMatriz`

```cpp
void imprimirMatriz(
    const vector<vector<int>>& matriz,
    const vector<char>& nombres
);
```

Imprime los nombres de los nodos como encabezados de filas y columnas.

`setw` se utiliza para mantener los valores alineados.

### `mostrarRepresentacionMatematica`

```cpp
void mostrarRepresentacionMatematica(
    const vector<vector<int>>& matriz,
    const vector<char>& nombres,
    TipoGrafo tipo
);
```

Muestra el conjunto de vértices:

```text
V = {A, B, C}
```

Para un grafo no dirigido muestra el conjunto de aristas:

```text
E = {{A,B}, {A,C}}
```

Solamente recorre la parte superior de la matriz para no mostrar dos veces una arista como `A-B` y `B-A`.

Para un grafo dirigido muestra el conjunto de arcos:

```text
A = {<A,B>, <C,A>}
```

En este caso sí importa el orden de los nodos.

### `buscarIndiceNodo`

```cpp
int buscarIndiceNodo(const vector<char>& nombres, char nombre);
```

Busca una letra dentro del vector de nombres.

Devuelve:

- El índice del nodo cuando lo encuentra.
- `-1` cuando el nodo no existe.

### `pedirNodoExistente`

```cpp
int pedirNodoExistente(
    const vector<char>& nombres,
    const string& mensaje
);
```

Solicita una letra y utiliza `buscarIndiceNodo` para comprobar que corresponda a un nodo registrado.

Si el nodo no existe, vuelve a pedirlo.

### `imprimirNodosEncontrados`

```cpp
void imprimirNodosEncontrados(
    const vector<char>& nombres,
    const vector<int>& indices
);
```

Recibe los índices encontrados durante una consulta y muestra las letras correspondientes.

Si el vector está vacío, imprime `Ninguno`.

### `mostrarAdyacentes`

```cpp
void mostrarAdyacentes(
    int indice,
    const vector<vector<int>>& matriz,
    const vector<char>& nombres,
    TipoGrafo tipo
);
```

En un grafo no dirigido, revisa la fila del nodo y muestra todos sus vecinos.

En un grafo dirigido distingue:

- Sucesores: nodos hacia los que sale una arista.
- Predecesores: nodos desde los que llega una arista.

Para encontrar sucesores se revisa una fila:

```cpp
matriz[indice][j]
```

Para encontrar predecesores se revisa una columna:

```cpp
matriz[j][indice]
```

### `pedirSecuenciaNodos`

```cpp
vector<int> pedirSecuenciaNodos(const vector<char>& nombres);
```

Solicita la longitud de una secuencia y después pide cada nodo.

La secuencia se guarda mediante índices para poder consultar directamente la matriz.

Si el usuario introduce:

```text
A B C D
```

la función podría devolver:

```text
0 1 2 3
```

### `esCamino`

```cpp
bool esCamino(
    const vector<int>& secuencia,
    const vector<vector<int>>& matriz
);
```

Una secuencia es un camino cuando cada par consecutivo está conectado.

Para la secuencia `A B C`, se comprueba:

```text
A con B
B con C
```

Si alguna de esas conexiones no existe, la secuencia no es un camino.

La longitud del camino es el número de nodos menos uno:

```text
A B C D
4 nodos - 1 = 3 aristas
```

### `esCaminoSimple`

```cpp
bool esCaminoSimple(const vector<int>& secuencia);
```

Comprueba que no se repitan nodos, con la excepción de que el último puede ser igual al primero.

Ejemplos:

```text
A B C D   -> camino simple
A B C A   -> puede ser camino simple y ciclo simple
A B A C   -> no es simple porque A se repite internamente
```

Se utiliza un `set` porque no permite valores repetidos.

### `tieneAristasDiferentes`

```cpp
bool tieneAristasDiferentes(const vector<int>& secuencia);
```

En un grafo no dirigido, un ciclo debe utilizar aristas diferentes.

La arista `A-B` se considera igual a `B-A`. Para normalizar cada arista se guarda primero el índice menor y después el mayor.

Por eso la secuencia:

```text
A B A
```

no se acepta como ciclo no dirigido: utiliza dos veces la misma arista.

### `esCiclo`

```cpp
bool esCiclo(
    const vector<int>& secuencia,
    const vector<vector<int>>& matriz,
    TipoGrafo tipo
);
```

Una secuencia forma un ciclo cuando:

- Contiene por lo menos tres posiciones.
- Es un camino válido.
- El primer nodo es igual al último.
- En un grafo no dirigido, no repite aristas.

Ejemplo:

```text
A B C A
```

### `analizarSecuencia`

```cpp
void analizarSecuencia(
    const vector<int>& secuencia,
    const vector<vector<int>>& matriz,
    TipoGrafo tipo
);
```

Agrupa las comprobaciones anteriores y presenta un resultado entendible:

```text
La secuencia es un camino simple. Longitud: 3 aristas.
También forma un ciclo simple.
```

O, si falta una conexión:

```text
La secuencia no es un camino.
```

### `generarArchivoDOT`

```cpp
bool generarArchivoDOT(
    const vector<vector<int>>& matriz,
    const vector<char>& nombres,
    TipoGrafo tipo,
    const string& nombreArchivo
);
```

Crea un archivo de texto llamado `grafo.dot`.

Para un grafo no dirigido genera algo similar a:

```dot
graph G {
    rankdir=LR;
    node [shape=circle];
    A;
    B;
    C;
    A -- B;
    A -- C;
}
```

Para un grafo dirigido utiliza `digraph` y el conector `->`.

Los nodos se declaran aunque estén aislados. Esto garantiza que también aparezcan en la gráfica.

### `mostrarMenu`

Presenta las operaciones disponibles:

```text
1. Mostrar matriz de adyacencia
2. Mostrar representación matemática
3. Consultar nodos adyacentes
4. Verificar camino o ciclo
5. Generar archivo gráfico
0. Salir
```

### `main`

`main` coordina el programa:

1. Solicita los datos básicos.
2. Crea la matriz llena de ceros.
3. Ingresa las conexiones.
4. Valida la matriz.
5. Muestra la información inicial.
6. Mantiene activo el menú hasta seleccionar `0`.

La lógica detallada permanece en funciones independientes, por lo que `main` se puede leer como un resumen del programa.

## 6. Diferencia entre grafos dirigidos y no dirigidos

### No dirigido

Una arista no tiene dirección:

```text
A -- B
```

Es equivalente a:

```text
B -- A
```

Su matriz debe ser simétrica:

```text
matriz[A][B] == matriz[B][A]
```

### Dirigido

Un arco sí tiene dirección:

```text
A -> B
```

No implica que exista:

```text
B -> A
```

Por eso su matriz puede ser asimétrica.

## 7. Ejemplo completo: grafo no dirigido

Se utilizarán tres nodos:

```text
A, B, C
```

Y tres aristas:

```text
A-B
A-C
B-C
```

### Respuestas de entrada

```text
Cantidad de nodos: 3
Nodo 1: A
Nodo 2: B
Nodo 3: C
Tipo: 1
A -- B: 1
A -- C: 1
B -- C: 1
```

### Matriz esperada

```text
      A  B  C
  A   0  1  1
  B   1  0  1
  C   1  1  0
```

### Representación matemática esperada

```text
V = {A, B, C}
E = {{A,B}, {A,C}, {B,C}}
```

### Consulta de adyacentes

Para el nodo `A`:

```text
Adyacentes de A: B, C
```

### Camino

Secuencia:

```text
A B C
```

Resultado:

```text
La secuencia es un camino simple. Longitud: 2 aristas.
No forma un ciclo.
```

### Ciclo

Secuencia:

```text
A B C A
```

Resultado:

```text
La secuencia es un camino simple. Longitud: 3 aristas.
También forma un ciclo simple.
```

## 8. Ejemplo completo: grafo dirigido

Nodos:

```text
A, B, C
```

Arcos:

```text
A -> B
B -> C
C -> A
```

### Matriz

```text
      A  B  C
  A   0  1  0
  B   0  0  1
  C   1  0  0
```

### Representación matemática

```text
V = {A, B, C}
A = {<A,B>, <B,C>, <C,A>}
```

### Consulta del nodo A

```text
Sucesores de A: B
Predecesores de A: C
```

### Secuencia

```text
A B C A
```

Forma un ciclo dirigido porque existen los tres arcos en el orden indicado.

La secuencia inversa `A C B A` no es un camino, salvo que también existan esos arcos en sentido contrario.

## 9. Uso de Graphviz

La opción 5 del menú crea:

```text
grafo.dot
```

Si Graphviz está instalado, se convierte en PNG con:

```powershell
dot -Tpng grafo.dot -o grafo.png
```

También se puede producir un archivo SVG:

```powershell
dot -Tsvg grafo.dot -o grafo.svg
```

Para comprobar si Graphviz está instalado:

```powershell
dot -V
```

La imagen generada se puede utilizar como evidencia en el informe.

## 10. Compilación y ejecución

Con `g++`:

```powershell
g++ -std=c++17 -Wall -Wextra -pedantic algoritmo.cpp -o algoritmo.exe
```

Para ejecutar:

```powershell
.\algoritmo.exe
```

Significado de las opciones:

- `-std=c++17`: utiliza el estándar C++17.
- `-Wall`: activa advertencias comunes.
- `-Wextra`: activa advertencias adicionales.
- `-pedantic`: detecta construcciones que no siguen estrictamente el estándar.
- `-o algoritmo.exe`: establece el nombre del ejecutable.

## 11. Casos de prueba recomendados

### Prueba 1: entradas inválidas

Intentar ingresar:

- Texto en la cantidad de nodos.
- `0` nodos.
- Más de 26 nodos.
- Dos nombres iguales.
- Un símbolo como nombre.
- Un valor diferente de `0` o `1` para una conexión.
- Una opción del menú fuera del rango.

El programa debe mostrar un mensaje y volver a solicitar el dato.

### Prueba 2: nodo aislado

Crear tres nodos y conectar únicamente `A-B`.

El nodo `C` debe:

- Aparecer en la matriz.
- Aparecer en el conjunto `V`.
- Mostrar `Ninguno` al consultar sus adyacentes.
- Aparecer en el archivo DOT.

### Prueba 3: camino inválido

Crear las conexiones:

```text
A-B
B-C
```

Probar la secuencia:

```text
A C
```

Debe responder que no es un camino.

### Prueba 4: ciclo no dirigido inválido

Crear únicamente `A-B` y probar:

```text
A B A
```

Es una secuencia conectada, pero no se considera ciclo no dirigido porque utiliza la misma arista dos veces.

### Prueba 5: dirección incorrecta

Crear solamente:

```text
A -> B
```

La secuencia `A B` debe ser camino. La secuencia `B A` no debe serlo.

## 12. Complejidad básica

### Memoria

La matriz utiliza espacio proporcional a:

```text
n × n
```

Por eso su complejidad espacial es:

```text
O(n²)
```

### Construcción

En un grafo dirigido se revisan casi todas las posiciones de la matriz:

```text
O(n²)
```

En uno no dirigido se revisa aproximadamente la mitad, pero su orden de complejidad continúa siendo:

```text
O(n²)
```

### Consultar una arista

Comprobar si existe una conexión entre dos nodos mediante la matriz cuesta:

```text
O(1)
```

### Consultar adyacentes

Se recorre una fila y, en grafos dirigidos, también una columna:

```text
O(n)
```

### Verificar un camino

Si la secuencia contiene `k` nodos, se revisan `k-1` conexiones:

```text
O(k)
```

## 13. Posibles preguntas durante la sustentación

### ¿Por qué utilizar una matriz de adyacencia?

Porque es la representación solicitada en la guía y permite consultar directamente si existe una conexión entre dos nodos.

### ¿Por qué la matriz de un grafo no dirigido es simétrica?

Porque si existe `A-B`, también existe `B-A`. Las posiciones `[A][B]` y `[B][A]` deben contener el mismo valor.

### ¿Por qué la diagonal contiene ceros?

Porque esta versión trabaja con grafos simples y no permite aristas desde un nodo hacia sí mismo.

### ¿Cuál es la diferencia entre adyacentes, sucesores y predecesores?

En un grafo no dirigido se habla de vecinos o adyacentes. En un grafo dirigido, los sucesores reciben una arista desde el nodo consultado y los predecesores envían una arista hacia él.

### ¿Cómo se determina si una secuencia es un camino?

Se consulta la matriz para verificar que cada par consecutivo de la secuencia esté conectado.

### ¿Cómo se determina si es un ciclo?

Primero debe ser un camino. Además, el primer nodo debe ser igual al último. En grafos no dirigidos no se permite repetir la misma arista.

### ¿Por qué no se usó POO?

Porque el alcance actual se resuelve claramente mediante funciones y un solo conjunto de datos. Una clase sería útil si el programa administrara varios grafos o creciera considerablemente.

### ¿Por qué se utilizó `const vector<...>&`?

La referencia evita copiar el vector completo y `const` garantiza que la función no lo modifique accidentalmente.

### ¿Por qué se genera un archivo DOT?

Porque permite representar grafos dirigidos y no dirigidos con poco código, y Graphviz se encarga de calcular automáticamente la posición de los nodos y dibujar las aristas.

## 14. Limitaciones conocidas

Estas limitaciones son intencionales para conservar el alcance del Laboratorio 3:

- Solo se utilizan letras de `A` a `Z`.
- El máximo es de 26 nodos.
- No se permiten lazos.
- No se permiten pesos.
- No se buscan caminos automáticamente; se valida una secuencia propuesta por el usuario.
- El programa genera DOT, pero necesita Graphviz para convertirlo en imagen.
- Los datos no se guardan para otra ejecución.
- No existe interfaz gráfica; se trabaja desde consola.

## 15. Evidencias recomendadas para el informe

Conviene tomar capturas de:

1. Ingreso de nodos y tipo de grafo.
2. Ingreso de las conexiones.
3. Matriz de adyacencia terminada.
4. Representación matemática.
5. Consulta de adyacentes.
6. Camino válido.
7. Camino inválido.
8. Ciclo válido.
9. Archivo DOT o imagen generada.
10. Una validación rechazando un dato incorrecto.

Cada captura debe acompañarse con una explicación corta del resultado.

## 16. Estructura sugerida del informe IEEE

El informe puede organizarse así:

1. **Título y autores.**
2. **Resumen:** propósito, método y resultado principal.
3. **Palabras clave:** grafos, matriz de adyacencia, camino, ciclo, C++.
4. **Introducción:** problema y utilidad de la teoría de grafos.
5. **Fundamento teórico:** vértices, aristas, grafos dirigidos, caminos, ciclos y adyacencia.
6. **Metodología:** decisiones de diseño y construcción del programa.
7. **Implementación:** estructuras de datos y funciones principales.
8. **Pruebas y resultados:** casos ejecutados y capturas.
9. **Análisis:** comportamiento, validaciones y limitaciones.
10. **Conclusiones:** cumplimiento de los objetivos y aprendizajes.
11. **Referencias:** bibliografía de la guía y otras fuentes utilizadas.
12. **Anexo:** código fuente completo.

## 17. Lista de verificación antes de entregar

- [ ] El código compila sin errores.
- [ ] No aparecen advertencias importantes.
- [ ] Se probó un grafo dirigido.
- [ ] Se probó un grafo no dirigido.
- [ ] Se probó un nodo aislado.
- [ ] Se comprobó un camino válido y uno inválido.
- [ ] Se comprobó un ciclo válido.
- [ ] Se generó `grafo.dot`.
- [ ] Se generó una imagen del grafo.
- [ ] Se guardaron capturas de las pruebas.
- [ ] El informe sigue el formato IEEE.
- [ ] El código está anexado al informe.
- [ ] Los nombres de los archivos son claros.
- [ ] No se incluyeron funciones de la Guía 4.

## 18. Archivos del proyecto

```text
algoritmo.cpp          Programa actual del Laboratorio 3.
PLAN_LAB3.md           Plan y etapas de desarrollo.
EXPLICACION_LAB3.md    Explicación completa del programa.
algoritmo_previo.cpp   Respaldo de la versión anterior.
```

Después de ejecutar la opción gráfica también puede aparecer:

```text
grafo.dot              Descripción del grafo para Graphviz.
grafo.png              Imagen generada con Graphviz.
```

## 19. Checklist para hacer el push

Primero se debe revisar el estado del repositorio:

```powershell
git status
```

Agregar únicamente los archivos que realmente se quieran publicar:

```powershell
git add algoritmo.cpp PLAN_LAB3.md EXPLICACION_LAB3.md
```

El archivo `algoritmo_previo.cpp` es un respaldo. Se puede conservar localmente o incluirlo solamente si el equipo considera útil mostrar la evolución. No es necesario para ejecutar la versión final.

Crear el commit:

```powershell
git commit -m "Implementar laboratorio 3 de teoria de grafos"
```

Finalmente:

```powershell
git push
```

Antes del `push`, es importante comprobar que no se estén agregando ejecutables, archivos temporales o imágenes innecesarias.

## 20. Resumen corto para explicar el proyecto

El programa permite construir un grafo simple dirigido o no dirigido mediante una matriz de adyacencia. Valida la cantidad y los nombres de los nodos, recibe cada conexión y garantiza la integridad de la matriz. Después permite mostrar la representación matemática, consultar adyacencias y verificar caminos y ciclos a partir de secuencias ingresadas por el usuario. Finalmente exporta la estructura a formato DOT para obtener una representación gráfica con Graphviz. La solución utiliza programación procedural y funciones pequeñas para mantener el código claro y fácil de explicar.
